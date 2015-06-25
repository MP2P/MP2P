#include <masks/messages.hh>
#include <network.hh>
#include <shared-buffer.hh>

namespace network
{
  using namespace boost::asio;
  using namespace network::masks;
  using copy = utils::shared_buffer::copy;

  Session::Session(ip::tcp::socket&& socket,
                   dispatcher_type dispatcher,
                   size_t id)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        dispatcher_{dispatcher},
        id_{id}
  {
    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "[" + std::to_string(id_) + "] " + "Opened session (tid=" + s.str() + ")";
  }

  Session::Session(io_service& io_service,
                   const std::string& host,
                   uint16_t port,
                   dispatcher_type dispatcher,
                   size_t id)
    : socket_{io_service},
      dispatcher_{dispatcher},
      id_{id}
  {
    ip::tcp::endpoint endpoint = network::endpoint_from_host(host, port);

    utils::Logger::cout() << "[" + std::to_string(id_) + "] " + "Opening session (" + host + " - "
                             + std::to_string(port) + ")";

    boost::system::error_code ec;
    socket_.connect(endpoint, ec); // Connect to the endpoint
    if (ec)
      throw std::logic_error("Unable to connect to server ("
            + ec.message() + ").");

    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "[" + std::to_string(id_) + "] " + "Opened session (tid=" + s.str() + ")";
  }

  Session::~Session()
  {
    // If the id == 0, then it means it was a moved-from object,
    // that became temporary, so there is no connection related to it.
    // Therefore, the client should not be notified of this destruction.
    if (id_ > 0)
      utils::Logger::cout() << "[" + std::to_string(id_) + "] " + "Closed session";
  }

  Session::Session(Session&& other)
    : socket_{std::move(other.socket_)},
      buff_(std::move(other.buff_)),
      dispatcher_{std::move(other.dispatcher_)},
      id_{std::move(other.id_)}
  {
    // Reset the id of the old session to 0.
    // This prevents incoherence in the book-keeping of the sessions.
    other.id_ = 0;
  }

  Session& Session::operator=(Session&& other)
  {
    socket_ = std::move(other.socket_);
    buff_ = std::move(other.buff_);
    dispatcher_ = std::move(other.dispatcher_);
    id_ = std::move(other.id_);

    // Reset the id of the old session to 0.
    // This prevents incoherence in the book-keeping of the sessions.
    other.id_ = 0;

    return *this;
  }

  void receive_header(std::shared_ptr<Session> s, std::function<void(const Packet&,
                                                  dispatcher_type)> receive_body,
                               dispatcher_type callback)
  {
    async_read(s->socket_get(),
        boost::asio::buffer(&*s->buff_get().begin(), s->buff_get().size()),
        transfer_exactly(sizeof (PACKET_HEADER)),
        [s, callback, receive_body](boost::system::error_code ec,
                                       std::size_t size_length)
        {
          if (!ec && size_length == sizeof (PACKET_HEADER))
          {
            const auto* header =
              reinterpret_cast<const PACKET_HEADER*>(s->buff_get().data());

            utils::Logger::cout() << "[" + std::to_string(s->id_get()) + "] " + "Receiving a message of size: "
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
            utils::Logger::cerr() << "[" + std::to_string(s->id_get()) + "] " + "Error while getting size: "
                                  + ec.message();
          }
        }
    );
  }

  void receive_message(std::shared_ptr<Session> s,
                       const Packet& p,
                       dispatcher_type callback)
  {
    async_read(s->socket_get(),
        p.message_seq_get()[0],
        transfer_exactly(p.size_get()),
        [s, p, callback](boost::system::error_code ec,
                            std::size_t /* length */)
        {
          if (!ec)
          {
            auto result = callback(p, *s);

            if (result == keep_alive::Yes)
              receive(s);
          }
          else
          {
            utils::Logger::cerr() << "[" + std::to_string(s->id_get()) + "] " + "Error: " + ec.message();
          }
        }
    );
  }

  void receive(std::shared_ptr<Session> s)
  {
    receive(s, s->dispatcher_get());
  }

  void receive(std::shared_ptr<Session> s, dispatcher_type callback)
  {
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    receive_header(s, std::bind(receive_message,
                             s,
                             std::placeholders::_1,
                             std::placeholders::_2),
                   callback
                  );
  }

  void blocking_receive(std::shared_ptr<Session> s)
  {
    blocking_receive(s, s->dispatcher_get());
  }

  void blocking_receive(std::shared_ptr<Session> s, dispatcher_type callback)
  {
    std::ostringstream ss;
    ss << std::this_thread::get_id();

    std::array<char, sizeof(masks::PACKET_HEADER)> packet_buff;

    try
    {
      read(s->socket_get(), boost::asio::buffer(&*packet_buff.begin(), packet_buff.size()));
    }
    catch (std::exception& e)
    {
      utils::Logger::cerr() << "[" + std::to_string(s->id_get()) + "] " + "Error while getting size: " + std::string(e.what());
      //kill(); // FIXME
      return;
    }

    const auto* header =
        reinterpret_cast<const PACKET_HEADER*>(packet_buff.data());
    utils::Logger::cout() << "[" + std::to_string(s->id_get()) + "] " + "Receiving a message of size: "
                             + std::to_string(header->size);
    Packet p{header->type.fromto, header->type.what,
             empty_message(header->size)};
    try
    {
      read(s->socket_get(), p.message_seq_get()[0], transfer_exactly(header->size));
    }
    catch (std::exception& e)
    {
      utils::Logger::cerr() << "[" + std::to_string(s->id_get()) + "] " + "Error: " + std::string(e.what());
      //kill(); // FIXME
      return;
    }

    auto result = callback(p, *s);

    if (result == keep_alive::Yes)
      blocking_receive(s, callback);
    //else if (result == keep_alive::No)
     // kill(); // FIXME
  }


  void send(std::shared_ptr<Session> s, const Packet& packet)
  {
    send(s, packet, [](auto, auto&){ return keep_alive::No; });
  }

  void send(std::shared_ptr<Session> s, const Packet& packet, dispatcher_type callback)
  {
    // FIXME : Ugly
    auto p = std::make_shared<Packet>(packet);
    auto& seq = p->message_seq_get();
    seq.insert(seq.begin(), packet.serialize_header());

    async_write(s->socket_get(),
        seq,
        [s, callback, p](boost::system::error_code ec,
                         std::size_t /* length */)
        {
          if (!ec)
          {
            /*auto result = */callback(*p, *s);

            //if (result == keep_alive::No)
             // kill(); // FIXME
          }
          else
          {
            utils::Logger::cerr() << "[" + std::to_string(s->id_get()) + "] " + "Error while sending: " + ec.message();
            //kill(); // FIXME : Get rid of Kill
          }
        }
    );
  }

  void blocking_send(std::shared_ptr<Session> s, const Packet& packet)
  {
    blocking_send(s, packet, [](auto, auto&){ return keep_alive::No; });
  }

  void blocking_send(std::shared_ptr<Session> s, const Packet& packet, dispatcher_type callback)
  {
    auto seq = packet.message_seq_get();
    seq.insert(seq.begin(), packet.serialize_header());
    try
    {
      write(s->socket_get(), seq);
      /*auto result = */callback(packet, *s);

      //if (result == keep_alive::No)
       // kill();
    }
    catch (std::exception& e)
    {
      utils::Logger::cerr() << "[" + std::to_string(s->id_get()) + "] " + "Error while sending: " + std::string(e.what());
      //kill();
    }
  }

  size_t Session::unique_id()
  {
    static std::atomic_size_t id{1};
    return id++;
  }

  void send_ack(Session& session,
                         const Packet& packet,
                         enum error_code ack)
  {
    const fromto_type fromto_src = packet.fromto_get();

    const fromto_type fromto_dst = fromto_inverse(fromto_src);

    const masks::ack response{ack};
    Packet to_send{fromto_dst, ack_w};
    to_send.add_message(&response, sizeof (response), copy::No);
    blocking_send(session.ptr(), to_send);
  }

  void recv_ack(Session& session)
  {
    blocking_receive(session.ptr(),
      [](Packet p, Session&)
      {
        CharT* data = p.message_seq_get().front().data();
        const ack* ack_code = reinterpret_cast<const ack*>(data);

        if (*ack_code != error_code::success)
        {
          std::ostringstream ss;
          ss << "Error : " << (int)*ack_code;
          throw std::logic_error(ss.str());
        }

        return keep_alive::No;
      }
    );
  }
}
