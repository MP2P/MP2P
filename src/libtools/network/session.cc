#include <network.hh>
#include <shared-buffer.hh>

using namespace boost::asio;

namespace network
{

  Session::Session(ip::tcp::socket&& socket,
                   std::function<error_code(Packet,Session&)> handler,
                   std::function<void(Session&)> delete_handler,
                   size_t id)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        handler_{std::move(handler)},
        delete_handler_{std::move(delete_handler)},
        id_{id}
  {
  }

  Session::Session(io_service& io_service,
                   const std::string& host,
                   const std::string& port,
                   std::function<error_code(Packet,Session&)> handler,
                   std::function<void(Session&)> delete_handler,
                   size_t id)
    : socket_{io_service},
      handler_{std::move(handler)},
      delete_handler_{std::move(delete_handler)},
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
        handler_{std::move(other.handler_)},
        delete_handler_{std::move(other.delete_handler_)},
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
    std::vector<unsigned char> v;
    Packet p{0, 0, 0, message_type{0}};
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
            const char* ch_buff =
              boost::asio::buffer_cast<const char*>(buff_.data());

            const auto* header =
              reinterpret_cast<const PACKET_HEADER*>(ch_buff);
            utils::Logger::cout() << "Receiving a message of size: "
                                     + std::to_string(header->size);

            Packet p{*header};

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

  void Session::receive_message(size_t msg_size, Packet p)
  {
    async_read(socket_,
               buffer(p.message_get().buffer_get()),
               transfer_exactly(msg_size),
               [this, msg_size, p](boost::system::error_code ec,
                                std::size_t length)
               {
                 if (!ec)
                 {
                   auto msg = p.message_get().string_get();
                   utils::Logger::cout() << "Received : " + msg;

                   length_ = length;
                   auto error = handler_(p, *this);
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
    auto str = packet.serialize();
    write(socket_, buffer(str.buffer_get()));
    auto error = handler_(packet, *this);
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
    delete_handler_(*this); // Ask the owner to delete
  }
}
