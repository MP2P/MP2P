#include <iostream>

#include <libtools.hh>


namespace network
{

  Session::Session(ip::tcp::socket&& socket, std::function<KeepAlive(Session&)> handler)
    : socket_{std::forward<ip::tcp::socket>(socket)},
      handler_{std::move(handler)}
  {
  }

  streambuf& Session::buff_get()
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
                       boost::asio::buffers_begin(bufs) + length_ - 2);
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
                                  '\n',
          [this](boost::system::error_code ec, std::size_t length)
          {
            if (!ec)
            {
              length_ = length;
              auto keep_alive = handler_(*this);
              if (keep_alive == KeepAlive::Die)
              {
                std::cout << "Closed session" << std::endl;
                socket_.close(); // Close the socket
              }
              else
                receive(); // Keep the socket alive
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
          utils::print(std::cout, w_mutex_, "Packet sent");
          auto keep_alive = handler_(*this);
          if (keep_alive == KeepAlive::Die)
            socket_.close(); // Close the socket
        }
      }
    );
  }
}
