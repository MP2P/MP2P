#include <network.hh>

#include <iostream>

namespace network
{

  Session::Session(ip::tcp::socket &&socket, std::function<std::unique_ptr<Error>(Session &)> handler)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        handler_{std::move(handler)}
  {
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
    std::string line;

    boost::asio::streambuf::const_buffers_type bufs = buff_.data();
    line = std::string(boost::asio::buffers_begin(bufs),
        boost::asio::buffers_begin(bufs) + length_);
    buff_.consume(length_);

    return line;
  }

  Packet Session::get_packet()
  {
    return Packet::deserialize(get_line());
  }

  // Read on the open socket
  void Session::receive()
  {
    std::cout << "Opened session (tid=" << std::this_thread::get_id() << ")" << std::endl;
    boost::asio::async_read_until(socket_,
        buff_,
        '|',
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            int msg_size = 0;
            boost::asio::streambuf::const_buffers_type bufs = buff_.data();
            std::string str(boost::asio::buffers_begin(bufs),
                  boost::asio::buffers_begin(bufs) + length);
            str.pop_back();
            msg_size = std::atoi(str.c_str());
            boost::asio::async_read(socket_,
                buff_,
                boost::asio::transfer_exactly(msg_size),
                [this, msg_size](boost::system::error_code ec, std::size_t length)
                {
                  if (!ec)
                  {
                    length_ = length + msg_size;
                    auto error = handler_(*this);
                    if (error->status_get() != Error::ErrorType::success)
                    {
                      std::cout << "Closed session" << std::endl;
                      socket_.close(); // Close the socket
                    }
                    else
                      receive(); // Keep the socket alive
                  }
                  else
                    std::cout << ec.message() << std::endl;
                }
            );
          }
        }
    );
  }

  // Send a packet on the open socket
  void Session::send(const Packet packet)
  {
    auto buff = boost::asio::buffer(packet.serialize());
    boost::asio::async_write(
        socket_,
        buff,
        [this](boost::system::error_code ec, std::size_t /* length */)
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
  }
}
