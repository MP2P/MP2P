#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include <libtools.hh>

using namespace boost::asio;

namespace network
{
  Client::Client(std::unique_ptr<libconfig::Config>&& config)
    : config_{std::move(config)},
      port_{utils::get_port(config_)},
      host_{"localhost"},
      socket_{io_service_}
  {
    std::ostringstream port;
    port << port_;
    ip::tcp::resolver resolver{io_service_};
    std::cout << "Port is : " << port.str() << std::endl;
    ip::tcp::resolver::query query{host_, port.str()};
    ip::tcp::resolver::iterator endpoint = resolver.resolve(query);
    boost::system::error_code ec;
    socket_.connect(*endpoint, ec);
  }

  Client::~Client()
  {}

  KeepAlive Client::handle(Session& session)
  {
    (void)session;
    std::cout << "Client handling" << std::endl;
    send(session);
    return KeepAlive::Live;
  }

  void Client::run()
  {
    auto session = std::make_shared<Session>(std::move(socket_),
        std::bind(&Client::handle, this, std::placeholders::_1));

    send(*session);
    io_service_.run();
  }

  void Client::send(Session& session)
  {
    std::string command;
    std::getline(std::cin, command);

    Packet p{4, 5, command};
    session.send(p);
  }

  void Client::stop()
  {

  }

}
