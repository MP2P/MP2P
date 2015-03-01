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
      host_{"localhost"}, // FIXME : Add hostname to the config file
      socket_{io_service_}
  {
    std::ostringstream msg;
    msg << "Endpoint host = " << host_;
    utils::print(std::cout, w_mutex_, msg.str());

    msg.str("");
    msg << "Endpoint port = " << port_;
    utils::print(std::cout, w_mutex_, msg.str());

    // Query needs the port as a string. Ugly fix.
    std::ostringstream port;
    port << port_;

    ip::tcp::resolver resolver{io_service_}; // Needed to resolve the host
    ip::tcp::resolver::query query{host_, port.str()};

    // Resolve the host and define the endpoint
    ip::tcp::resolver::iterator endpoint = resolver.resolve(query);

    // FIXME : Check for errors
    boost::system::error_code ec;
    socket_.connect(*endpoint, ec); // Connect to the endpoint
  }

  Client::~Client()
  {
    // FIXME : Close sockets and everything if needed
  }

  KeepAlive Client::handle(Session& session)
  {
    (void)session;
    std::ostringstream msg;
    msg << "Client handling";
    utils::print(std::cout, w_mutex_, msg.str());
    send(session); // Ask for a new command
    return KeepAlive::Live;
  }

  void Client::run()
  {
    // Create a shared_ptr to prevent losing the object after exiting the scope
    auto session = std::make_shared<Session>(std::move(socket_),
        std::bind(&Client::handle, this, std::placeholders::_1));

    send(*session); // Ask for a command
    io_service_.run();
  }

  void Client::send(Session& session)
  {
    std::string command;
    std::getline(std::cin, command);

    Packet p{4, 5, command}; // Create a Packet containing the command
    session.send(p);
  }

  void Client::stop()
  {
    // FIXME : Stop everything, join threads if needed
  }

}
