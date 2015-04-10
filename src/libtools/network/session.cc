#include <network.hh>

namespace network
{

  Session::Session(ip::tcp::socket &&socket, std::function<error_code(Session &)> handler)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        handler_{std::move(handler)}
  {
  }

  Session::Session(boost::asio::io_service& io_service,
                   const std::string& host,
                   const std::string& port,
                   std::function<error_code(Session &)> handler)
    : socket_{io_service},
      handler_{std::move(handler)}
  {
    boost::asio::ip::tcp::resolver resolver{io_service}; // Resolve the host
    boost::asio::ip::tcp::resolver::query query{host, port};
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = *iter;

    boost::system::error_code ec;
    socket_.connect(endpoint, ec); // Connect to the endpoint
    if (ec)
      throw std::logic_error("Unable to connect to server");

    std::cout << "Opened session (tid=" << std::this_thread::get_id() << ")" << std::endl;
  }

  ip::tcp::socket& Session::socket_get()
  {
    return socket_;
  }

  streambuf &Session::buff_get()
  {
    return buff_;
  }

  size_t Session::length_get()
  {
    return length_;
  }

  // FIXME : Get a generic function to do this
  std::string Session::get_line()
  {
    boost::asio::streambuf::const_buffers_type bufs = buff_.data();

    return std::string(boost::asio::buffers_begin(bufs),
        boost::asio::buffers_begin(bufs) + length_);
  }

  Packet Session::get_packet()
  {
    return Packet::deserialize(get_line());
  }

  // Read on the open socket
  void Session::receive()
  {
    utils::Logger::cout() << "Session recieving...(tid=" << std::this_thread::get_id()
              << ")";
    boost::asio::async_read(socket_,
        buff_,
        boost::asio::transfer_exactly(sizeof(uint32_t)),
        [this](boost::system::error_code ec, std::size_t size_length)
        {
          if (!ec)
          {
            boost::asio::streambuf::const_buffers_type bufs = buff_.data();

            std::string str(boost::asio::buffers_begin(bufs),
                            boost::asio::buffers_begin(bufs) + size_length);

            const uint32_t* sizep =
              reinterpret_cast<const uint32_t*>(str.c_str());

            uint32_t msg_size = *sizep;

            std::cout << "Receiving a message of size: "
                      << msg_size << std::endl;

            // Read the whole message + the headers left
            boost::asio::async_read(socket_,
                buff_,
                // FIXME : remove hardcoded size
                boost::asio::transfer_exactly(msg_size + 2 * sizeof (uint8_t)),
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

                    (void)error;
                    /* FIXME : Specify error codes
                    if (error->status_get() != Error::ErrorType::success)
                    {
                      std::cout << "Closed session" << std::endl;
                      socket_.close(); // Close the socket
                    }
                    else
                    */
                      receive(); // Keep the socket alive
                  }
                  else
                  {
                    std::cout << "Error: " << ec.message() << std::endl;
                  }
                }
            );
          }
          else
          {
            std::cout << "Error while getting size: " << ec.message()
                      << std::endl;

            }
        }
    );
  }

  // Send a packet on the open socket
  void Session::send(const Packet packet)
  {
    auto str = packet.serialize();
    boost::asio::write(socket_, boost::asio::buffer(str));
    // FIXME : Check wether it's closed
    /*auto error = handler_(*this);
    if (error->status_get() != Error::ErrorType::success)
      socket_.close(); // Close the socket
      */
  }
}
