#include <network.hh>

namespace network
{
  using namespace boost::asio;

  Server::Server(boost::asio::ip::address_v6 addr, uint16_t port,
                 io_service &io_service,
                 dispatcher_type dispatcher)
      : acceptor_{io_service},
        socket_{io_service},
        dispatcher_{dispatcher}
  {
    // Use of ipv6 by default, with IPV6_V6ONLY disabled, it will listen to
    // both ipv4 & ipv6.
    // ipv4 addresses will be mapped to ipv6 like this: `::ffff:192.168.0.'
    ip::tcp::endpoint endpoint(addr, port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint); // Throws
    acceptor_.listen();
    utils::Logger::cout() << "Successfully binded " + addr.to_string()
                             + "/" + std::to_string(port);
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

  bool Server::is_running()
  {
    return acceptor_.is_open();
  }

  void Server::listen()
  {
    utils::Logger::cout() << "Listening...";

    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::ostringstream s;
            s << std::this_thread::get_id();
            utils::Logger::cout() << "Connection accepted. (Thread "
                                      + s.str() + ").";

            size_t id = Session::unique_id();

            auto session = Session::create(std::move(socket_), dispatcher_, id);

            receive(session);

            // At the end of each request & treatment, we call listen again.
            listen();
          }
        }
    );
  }
}
