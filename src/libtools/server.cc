#include <iostream>

#include <libtools.hh>


namespace network
{
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
    stop();
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
