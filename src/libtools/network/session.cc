#include <network.hh>

#include <iostream>

namespace network
{

  Session::Session(ip::tcp::socket &&socket, std::function<std::unique_ptr<Error>(Session &)> handler)
    : socket_{std::forward<ip::tcp::socket>(socket)},
    handler_{std::move(handler)}
  {
    std::cout << "Opened session (tid=" << std::this_thread::get_id() << ")" << std::endl;
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
    std::cout << "Session recieving...(tid=" << std::this_thread::get_id()
              << ")" << std::endl;
    boost::asio::async_read_until(socket_,
        buff_,
        '|', // FIXME : transfer the size first
        [this](boost::system::error_code ec, std::size_t size_length)
        {
          if (!ec)
          {
            boost::asio::streambuf::const_buffers_type bufs = buff_.data();
            std::string str(boost::asio::buffers_begin(bufs),
                            boost::asio::buffers_begin(bufs) + size_length);
            str.pop_back();

            auto msg_size = std::stoll(str);

            std::cout << "Receiving a message of size: "
                      << msg_size << std::endl;

            // Read the whole message + the headers left
            boost::asio::async_read(socket_,
                buff_,
                // FIXME : remove hardcoded size
                boost::asio::transfer_exactly(msg_size + 4),
                [this, msg_size, size_length](boost::system::error_code ec, std::size_t length)
                {
                  if (!ec)
                  {
                    length_ = length + size_length;

                    auto error = handler_(*this);

                    // Reset buffer and length to be ready for reading again
                    buff_.consume(length_);
                    length_ = 0;

                    if (error->status_get() != Error::ErrorType::success)
                    {
                      std::cout << "Closed session" << std::endl;
                      socket_.close(); // Close the socket
                    }
                    else
                      receive(); // Keep the socket alive
                  }
                  else
                    std::cout << "Error: " << ec.message() << std::endl;
                }
            );
          }
        }
    );
  }

  // Send a packet on the open socket
  void Session::send(const Packet packet)
  {
    auto str = packet.serialize();
    std::cout << "Gonna send this : " << str << std::endl;
    socket_.send(boost::asio::buffer(str));
    auto error = handler_(*this);
    if (error->status_get() != Error::ErrorType::success)
      socket_.close(); // Close the socket
  }
}
