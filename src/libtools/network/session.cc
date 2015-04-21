#include <network.hh>

using namespace boost::asio;

namespace network
{

  Session::Session(ip::tcp::socket&& socket,
                   std::function<error_code(Session&)> handler,
                   std::function<void(Session&)> delete_handler)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        handler_{std::move(handler)},
        delete_handler_{std::move(delete_handler)}

  {
  }

  Session::~Session()
  {
    utils::Logger::cout() << "Session destroyed";
  }

  Session::Session(io_service& io_service,
                   const std::string& host,
                   const std::string& port,
                   std::function<error_code(Session&)> handler,
                   std::function<void(Session&)> delete_handler)
    : socket_{io_service},
      handler_{std::move(handler)},
      delete_handler_{std::move(delete_handler)}
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

  // FIXME : Get a generic function to do this
  std::string Session::get_line()
  {
    streambuf::const_buffers_type bufs = buff_.data();

    return std::string(buffers_begin(bufs),
        buffers_begin(bufs) + length_);
  }

  Packet Session::get_packet()
  {
    return deserialize(get_line());
  }

  // Read on the open socket
  void Session::receive()
  {
    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Session receiving...(tid=" + s.str() + ")";
    async_read(socket_,
        buff_,
        transfer_exactly(sizeof (uint32_t)),
        [this](boost::system::error_code ec, std::size_t size_length)
        {
          if (!ec)
          {
            streambuf::const_buffers_type bufs = buff_.data();

            std::string str(buffers_begin(bufs),
                            buffers_begin(bufs) + size_length);

            const uint32_t* sizep =
              reinterpret_cast<const uint32_t*>(str.c_str());

            uint32_t msg_size = *sizep;

            utils::Logger::cout() << "Receiving a message of size: "
                                     + std::to_string(msg_size);

            // Read the whole message + the headers left
            async_read(socket_,
                buff_,
                transfer_exactly(msg_size + 2 * sizeof (uint8_t)),
                [this, msg_size, size_length](boost::system::error_code ec,
                                              std::size_t length)
                {
                  if (!ec)
                  {
                    length_ = length + size_length;

                    auto error = handler_(*this);

                    // Reset buffer and length to be ready for reading again
                    buff_.consume(length_);
                    length_ = 0;

                    if (error == 100)
                    {
                      utils::Logger::cout() << "Closed session";
                      socket_.close(); // Close the socket
                      delete_handler_(*this); // Ask the owner to delete
                    }
                    else
                    {
                      receive(); // Keep the socket alive
                    }
                  }
                  else
                  {
                    utils::Logger::cerr() << "Error: " + ec.message();
                  }
                }
            );
          }
          else
          {
            utils::Logger::cerr() << "Error while getting size: "
                                  + ec.message();
          }
        }
    );
  }

  // Send a packet on the open socket
  void Session::send(const Packet& packet)
  {
    auto str = packet.serialize();
    write(socket_, buffer(str));
    auto error = handler_(*this);
    if (error == 1)
      socket_.close();
  }
}
