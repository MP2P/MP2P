#include <network.hh>
#include <shared-buffer.hh>

using namespace boost::asio;

namespace network
{

  Session::Session(ip::tcp::socket&& socket,
                   std::function<error_code(Packet,Session&)> dispatcher,
                   std::function<void(Session&)> delete_dispatcher,
                   size_t id)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        dispatcher_{std::move(dispatcher)},
        delete_dispatcher_{std::move(delete_dispatcher)},
        id_{id}
  {
  }

  Session::Session(io_service& io_service,
                   const std::string& host,
                   const std::string& port,
                   std::function<error_code(Packet,Session&)> dispatcher,
                   std::function<void(Session&)> delete_dispatcher,
                   size_t id)
    : socket_{io_service},
      dispatcher_{std::move(dispatcher)},
      delete_dispatcher_{std::move(delete_dispatcher)},
      id_{id}
  {
    ip::tcp::resolver resolver{io_service}; // Resolve the host
    ip::tcp::resolver::query query{host, port};
    ip::tcp::resolver::iterator iter = resolver.resolve(query);
    ip::tcp::endpoint endpoint = *iter;

    boost::system::error_code ec;
    socket_.connect(endpoint, ec); // Connect to the endpoint
    if (ec)
      throw std::logic_error("Unable to connect to server");

    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Opened session (tid=" + s.str() + ")";
  }

  Session::Session(Session&& other)
      : socket_{std::move(other.socket_)},
        length_{other.length_},
        dispatcher_{std::move(other.dispatcher_)},
        delete_dispatcher_{std::move(other.delete_dispatcher_)},
        id_{other.id_}
  {
  }

  // FIXME : Get a generic function to do this
  std::string Session::get_line()
  {
    streambuf::const_buffers_type bufs = buff_.data();

    return std::string(buffers_begin(bufs),
        buffers_begin(bufs) + length_);
  }

  Packet Session::get_packet()
  {
    // FIXME : Create a real packet from the buffer
    std::vector<char> v;
    Packet p{0, 0, 0, message_type{&*v.begin(), 0, true}};
    return p;
    //return deserialize(get_line());
  }

  void Session::receive_header(std::function<void(size_t, Packet)> callback)
  {
    async_read(socket_,
        buff_,
        transfer_exactly(sizeof (PACKET_HEADER)),
        [this, callback](boost::system::error_code ec, std::size_t size_length)
        {
          if (!ec && size_length == sizeof (PACKET_HEADER))
          {
            const CharT* ch_buff = buffer_cast<const CharT*>(buff_.data());

            const auto* header =
              reinterpret_cast<const PACKET_HEADER*>(ch_buff);

            utils::Logger::cout() << "Receiving a message of size: "
                                     + std::to_string(header->size);

            Packet p{*header}; // Create a packet with the read header
            // Allocate a buffer to read the message into
            p.add_message(empty_message(header->size));

            // Read the whole message + the headers left
            callback(header->size, p);
          }
          else if (ec == boost::asio::error::eof)
            kill();
          else
            utils::Logger::cerr() << "Error while getting size: "
                                  + ec.message();
        }
    );
  }

  void Session::receive_message(size_t msg_size, const Packet& p)
  {
    async_read(socket_,
               p.message_seq_get()[0],
               transfer_exactly(msg_size),
               [this, msg_size, p](boost::system::error_code ec,
                                std::size_t length)
               {
                 if (!ec)
                 {
                   length_ = length;
                   auto error = dispatcher_(p, *this);
                   length_ = 0;

                   if (error == 100)
                     kill();
                   else
                     receive(); // Keep the socket alive
                 }
                 else
                 {
                   utils::Logger::cerr() << "Error: " + ec.message();
                   kill();
                 }
               }
    );
  }

  // Read on the open socket
  void Session::receive()
  {
    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Session receiving...(tid=" + s.str() + ")";
    receive_header(std::bind(&Session::receive_message,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2));
  }

  // Send a packet on the open socket
  void Session::send(const Packet& packet)
  {
    auto seq = packet.message_seq_get();
    seq.insert(seq.begin(), packet.serialize_header());
    write(socket_, seq);
    auto error = dispatcher_(packet, *this);
    if (error == 1)
      socket_.close();
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
}
