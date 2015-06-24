#include <masks/messages.hh>
#include <network.hh>
#include <shared-buffer.hh>

namespace network
{
  using namespace boost::asio;
  using namespace network::masks;
  using copy = utils::shared_buffer::copy;

  Session::Session(ip::tcp::socket&& socket,
                   dispatcher_type recv_dispatcher,
                   dispatcher_type send_dispatcher,
                   std::function<void(Session&)> delete_dispatcher,
                   size_t id)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        recv_dispatcher_{recv_dispatcher},
        send_dispatcher_{send_dispatcher},
        delete_dispatcher_{delete_dispatcher},
        id_{id}
  {
  }

  Session::Session(io_service& io_service,
                   const std::string& host,
                   uint16_t port,
                   dispatcher_type recv_dispatcher,
                   dispatcher_type send_dispatcher,
                   std::function<void(Session&)> delete_dispatcher,
                   size_t id)
    : socket_{io_service},
      recv_dispatcher_{recv_dispatcher},
      send_dispatcher_{send_dispatcher},
      delete_dispatcher_{delete_dispatcher},
      id_{id}
  {
    ip::tcp::endpoint endpoint = network::endpoint_from_host(host, port);

    utils::Logger::cout() << "Opening session (" + host + " - "
                             + std::to_string(port) + ")";

    boost::system::error_code ec;
    socket_.connect(endpoint, ec); // Connect to the endpoint
    if (ec)
      throw std::logic_error("Unable to connect to server ("
            + ec.message() + ").");

    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Opened session (tid=" + s.str() + ")";
  }

  void Session::receive_header(std::function<void(const Packet&,
                                                  dispatcher_type)> receive_body,
                               dispatcher_type callback)
  {
    async_read(socket_,
        boost::asio::buffer(&*buff_.begin(), buff_.size()),
        transfer_exactly(sizeof (PACKET_HEADER)),
        [this, callback, receive_body](boost::system::error_code ec,
                                       std::size_t size_length)
        {
          if (!ec && size_length == sizeof (PACKET_HEADER))
          {
            const auto* header =
              reinterpret_cast<const PACKET_HEADER*>(buff_.data());

            utils::Logger::cout() << "Receiving a message of size: "
                                     + std::to_string(header->size);

            // We're not using the constructor of packet with the full header
            // like Packet{*header} because we don't want the size to
            // be specified inside the header yet.
            // Adding the empty message is going to update the size
            Packet p{header->type.fromto, header->type.what};

            // Allocate a buffer to read the message into
            p.add_message(empty_message(header->size));

            // Read the whole message
            receive_body(p, callback);
          }
          else if (ec != boost::asio::error::eof)
          {
            utils::Logger::cerr() << "Error while getting size: "
                                  + ec.message();
            // Kill the session if an error occured
            kill(); // FIXME : Kill is not the right approach.
          }
        }
    );
  }

  void Session::receive_message(const Packet& p,
                                dispatcher_type callback)
  {
    async_read(socket_,
        p.message_seq_get()[0],
        transfer_exactly(p.size_get()),
        [this, p, callback](boost::system::error_code ec,
                            std::size_t length)
        {
          if (!ec)
          {
            length_ = length;
            auto result = callback(p, *this);
            length_ = 0;

            process_result(result, p, [this](){ receive(); });
          }
          else
          {
            utils::Logger::cerr() << "Error: " + ec.message();
            kill(); // FIXME : Get rid of Kill
          }
        }
    );
  }

  void Session::receive()
  {
    receive(recv_dispatcher_);
  }

  void Session::receive(dispatcher_type callback)
  {
    std::ostringstream s;
    s << std::this_thread::get_id();
    receive_header(std::bind(&Session::receive_message,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2),
                   callback
                  );
  }

  void Session::blocking_receive()
  {
    blocking_receive(recv_dispatcher_);
  }

  void Session::blocking_receive(dispatcher_type callback)
  {
    std::ostringstream s;
    s << std::this_thread::get_id();

    std::array<char, sizeof(masks::PACKET_HEADER)> packet_buff;

    read(socket_, boost::asio::buffer(&*packet_buff.begin(), packet_buff.size()));

    const auto* header =
        reinterpret_cast<const PACKET_HEADER*>(packet_buff.data());
    utils::Logger::cout() << "Receiving a message of size: "
                             + std::to_string(header->size);
    Packet p{header->type.fromto, header->type.what,
             empty_message(header->size)};
    read(socket_, p.message_seq_get()[0], transfer_exactly(header->size));

    auto result = callback(p, *this);
    process_result(result, p, [this, callback](){ blocking_receive(callback); });
  }

  void Session::send(const Packet& packet)
  {
    send(packet, send_dispatcher_);
  }

  void Session::send(const Packet& packet, dispatcher_type callback)
  {
    auto p = std::make_shared<Packet>(packet);
    auto& seq = p->message_seq_get();
    seq.insert(seq.begin(), packet.serialize_header());

    async_write(socket_,
        seq,
        [this, callback, p](boost::system::error_code ec,
                         std::size_t length)
        {
          if (!ec)
          {
            length_ = length;
            auto result = callback(*p, *this);
            length_ = 0;

            process_result(result, *p,
                           [this, p, callback](){ send(*p, callback); });
          }
          else
          {
            utils::Logger::cerr() << "Error while sending: " + ec.message();
            kill(); // FIXME : Get rid of Kill
          }
        }
    );
  }

  void Session::blocking_send(const Packet& packet)
  {
    blocking_send(packet, send_dispatcher_);
  }

  void Session::blocking_send(const Packet& packet, dispatcher_type callback)
  {
    auto seq = packet.message_seq_get();
    seq.insert(seq.begin(), packet.serialize_header());
    try
    {
      write(socket_, seq);
      auto result = callback(packet, *this);
      process_result(result, packet,
                     [this, &packet, callback](){ blocking_send(packet, callback); });
    }
    catch (std::exception& e)
    {
      utils::Logger::cerr() << "Error while sending: " + std::string(e.what());
      kill();
    }
  }

  size_t Session::unique_id()
  {
    static std::atomic_size_t id;
    return id++;
  }

  void Session::kill()
  {
    utils::Logger::cout() << "Closed session";
    socket_.close(); // Close the socket
    delete_dispatcher_(*this); // Ask the owner to delete
  }

  void Session::send_ack(Session& session,
                         const Packet& packet,
                         enum error_code ack)
  {
    const fromto_type fromto_src = packet.fromto_get();

    const fromto_type fromto_dst = fromto_inverse(fromto_src);

    const masks::ack response{ack};
    Packet to_send{fromto_dst, ack_w};
    to_send.add_message(&response, sizeof (response), copy::No);
    session.blocking_send(to_send);
  }
}
