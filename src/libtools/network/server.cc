#include <network.hh>
#include <utils.hh>

#include <iostream>

using namespace boost::asio;

namespace network
{
  Server::Server(io_service &io_service,
      std::function<error_code(Packet, Session &)> handler)
      : acceptor_{io_service},
        socket_{io_service},
        handler_{std::move(handler)}
  {
    // Use of ipv6 by default, with IPV6_V6ONLY disabled, it will listen to
    // both ipv4 & ipv6.
    // ipv4 addresses will be mapped to ipv6 like this: `::ffff:192.168.0.'
    unsigned port = utils::Conf::get_instance().port_get();
    ip::tcp::endpoint endpoint(ip::tcp::v6(), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    try
    {
      acceptor_.bind(endpoint);
      acceptor_.listen();
      listen(); // Listen for new connections
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      acceptor_.cancel();
    }
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
            sessions_.emplace(id,
              Session{std::move(socket_), handler_,
                std::bind(&Server::delete_handler, this, std::placeholders::_1),
                id
              }
            );

            auto& inserted = sessions_.at(id);
            inserted.receive();

            // At the end of each request & treatment, we call listen again.
            listen();
          }
        }
    );
  }

  void Server::delete_handler(Session& session)
  {
    sessions_.erase(session.id_get());
  }
}
