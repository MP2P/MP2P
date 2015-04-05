#include <network.hh>

namespace network
{

  Session::Session(ip::tcp::socket &&socket, std::function<error_code(Session &)> handler)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        handler_{std::move(handler)}
  {
  }

  streambuf &Session::buff_get()
  {
    return buff_;
  }

  size_t Session::length_get()
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
    //std::cout << "Opened session (tid=" << std::this_thread::get_id() << ")" << std::endl;
    utils::Logger::cout() << "Opened session (tid=" << std::this_thread::get_id() << ").";
    boost::asio::async_read_until(socket_,
        buff_,
        '\n',
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            length_ = length;
            // Calling master.handle(session)
            /*error_code error = */handler_(*this);
            //if (error->status_get() != Error::ErrorType::success)
            //{
              socket_.close(); // Close the socket
              //Send an error packet.
              //std::cout << "Closed session" << std::endl;
              utils::Logger::cout() << "Closed session.";
            //}
            //else
            //  receive(); // Keep the socket alive
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
            //std::cout << "Packet sent!" << std::endl;
            utils::Logger::cout() << "Packet sent!";
            /*error_code error = */handler_(*this);
            //if (error->status_get() != Error::ErrorType::success)
              socket_.close(); // Close the socket
          }
        }
    );
  }
}
