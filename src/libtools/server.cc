#include <iostream>

#include <libtools.hh>


namespace network
{
  Server::Server(io_service& io_service, const unsigned port, std::function<void()> handler)
    : acceptor_{io_service},
      socket_{io_service},
      handler_{handler}
  {
    // Use of ipv6 by default, with IPV6_V6ONLY disabled, it will listen to
    // both ipv4 & ipv6. 
    // ipv4 addresses will be mapped to ipv6 like this: `::ffff:192.168.0.'
    ip::tcp::endpoint endpoint(ip::tcp::v6(), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    listen();
  }


  Server::~Server()
  {}

  void Server::listen()
  {
    std::cout << "Listening" << std::endl;
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            handler_();

            // At the end of each request & treatment, we call listen again.
            listen();
          }
        }
    );
  }
}
