#include <iostream>

#include <libtools.hh>


namespace network
{
  void handle_master()
  {
    std::cout << "Master handling..." << std::endl;
  }
  void handle_storage()
  {
    std::cout << "Storage handling..." << std::endl;
  }

  void handle_zeus()
  {
    std::cout << "Zeus handling..." << std::endl;
  }

  auto get_handle_fun(Node node)
  {
    if (node == MASTER)
      return handle_master;
    else if (node == STORAGE)
      return handle_storage;
    return handle_zeus;
  }

  Server::Server(io_service& io_service, const unsigned port, Node node)
    : acceptor_{io_service},
      socket_{io_service}
  {
    // Use of ipv6 by default, with IPV6_V6ONLY disabled, it will listen to
    // both ipv4 & ipv6. 
    // ipv4 addresses will be mapped to ipv6 like this: `::ffff:192.168.0.'
    ip::tcp::endpoint endpoint(ip::tcp::v6(), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    handle(node);
  }


  Server::~Server()
  {}

  void Server::handle(Node node)
  {
    std::cout << "Handling" << std::endl;
    acceptor_.async_accept(socket_,
        [this, node](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::cout << "Connection accepted. (Thread "
                      << std::this_thread::get_id() << ")" << std::endl;

            /// Call to the needed function.
            get_handle_fun(node)();

            /// At the end of each request & treatment, we call listen again.
            handle(node);
          }
        }
    );
  }
}
