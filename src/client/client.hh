#pragma once

#include <libconfig.h++>
#include <network.hh>

using namespace network;

using error_code = uint16_t;

class Client
{
private:
  io_service io_service_; // Does not requires instantiation
  ip::tcp::socket socket_;

  error_code handle(Session &session);
  void send(Session &session);

public:
  Client();
  ~Client();

  // Creates threads & make them bind the same port defined in config.
  void run();

  // Causes the server to stop it's running threads if any.
  void stop();
};
