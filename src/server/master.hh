#pragma once

#include <forward_list>
#include <thread>
#include <boost/asio.hpp>

#include <network.hh>

using namespace network;
using namespace boost::asio;
using namespace boost::posix_time;

class Master
{
private:
  io_service io_service_; // Does not need instantiation
  network::Server server_;
  std::forward_list<std::thread> threads_;

  std::unique_ptr<Error> handle(Session &session);

  // Causes the server to stop it's running threads if any.
  void stop();

public:
  Master();
  ~Master();

  // Creates threads & make them bind the same port defined in config.
  bool run();

  // Catch a CTRL+C / CTRL+D signal, call Master::stop(); and exit.
  void catch_stop();
};

std::unique_ptr<Error> Handle_CM(Packet & packet, Session & session);
std::unique_ptr<Error> Handle_SM(Packet & packet, Session & session);
std::unique_ptr<Error> Handle_MM(Packet & packet, Session & session);
