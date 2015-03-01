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

  // Read on the open socket
  void Session::receive()
  {
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
                socket_.close(); // Close the socket
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
    boost::asio::async_write(socket_,
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

  Server::Server(io_service& io_service,
                 const unsigned port,
                 std::function<KeepAlive(Session&)> handler)
    : acceptor_{io_service},
      socket_{io_service},
      handler_{std::move(handler)}
  {
    // Use of ipv6 by default, with IPV6_V6ONLY disabled, it will listen to
    // both ipv4 & ipv6.
    // ipv4 addresses will be mapped to ipv6 like this: `::ffff:192.168.0.'
    ip::tcp::endpoint endpoint(ip::tcp::v6(), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    listen(); // Listen for new connections
  }


  Server::~Server()
  {
    // FIXME : There should be something to do here.
  }

  void Server::stop()
  {
    if (acceptor_.is_open())
      acceptor_.cancel();
  }

  void Server::listen()
  {
    std::ostringstream msg;
    msg << "Listening";
    std::mutex tmp;
    std::make_unique<libconfig::Config>();
    utils::print(std::cout, tmp, msg.str());
    acceptor_.async_accept(socket_,
      [this](boost::system::error_code ec)
      {
        if (!ec)
          {
            std::mutex tmp;
            std::ostringstream msg;
            msg.str("");
            msg << "Connection accepted. (Thread "
                      << std::this_thread::get_id() << ")";
            utils::print(std::cout, tmp, msg.str());
            auto session = std::make_shared<Session>(std::move(socket_), handler_);
            session->receive();
            sessions_.push_back(session);

            // At the end of each request & treatment, we call listen again.
            listen();
          }
        }
    );
  }
}
