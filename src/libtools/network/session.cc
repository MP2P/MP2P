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

  unsigned Session::length_get()
  {
    return length_;
  }

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
    std::cout << "Buffer size is : " << buff_.size() << std::endl;
    buff_.prepare(1024);
    boost::asio::async_read(socket_,
        buff_,
        boost::asio::transfer_exactly(2),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
    std::cout << "Buffer size is : " << buff_.size() << std::endl;
    std::cout << "len is : " << length << std::endl;
            int msg_size = 0;
            boost::asio::streambuf::const_buffers_type bufs = buff_.data();
            std::string str(boost::asio::buffers_begin(bufs),
                            boost::asio::buffers_begin(bufs) + length);
            std::cout << "SIZE BUFFER IS : " << str << std::endl;
            //buff_.consume(2);
            str.pop_back();
            std::cout << "STR IS:" << str << std::endl;
            msg_size = std::stoi(str);
            std::cout << "msg_size IS:" << msg_size << std::endl;
            boost::asio::async_read(socket_,
                buff_,
                boost::asio::transfer_exactly(msg_size + 6),
                [this, msg_size](boost::system::error_code ec, std::size_t /*length*/)
                {
                  if (!ec)
                  {
                    length_ = msg_size + 6;
                    boost::asio::streambuf::const_buffers_type bufs = buff_.data();
                    std::string str(boost::asio::buffers_begin(bufs),
                    boost::asio::buffers_begin(bufs) + length_);
                    std::cout << "BUFFER IS : " << str << std::endl;
                    auto error = handler_(*this);
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
/*    boost::asio::async_write(
        socket_,
        buff_,
        [this](boost::system::error_code ec, std::size_t / * length * /)
        {
          if (!ec)
          {
            //utils::print(std::cout, w_mutex_, "Packet sent");
            std::cout << "Packet sent!" << std::endl;
            auto error = handler_(*this);
            if (error->status_get() != Error::ErrorType::success)
              socket_.close(); // Close the socket
          }
        }
    );
*/
  }
}
