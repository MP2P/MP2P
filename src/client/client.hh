#pragma once

#include <libconfig.h++>
#include <network.hh>

using namespace network;

using error_code = uint16_t;

class Client
{
private:
  io_service io_service_; // Does not requires instantiation
  Session master_session_;

  error_code handle(Session &session);

public:
  Client();

  // Creates threads & make them bind the same port defined in config.
  void run();

  // Causes the server to stop it's running threads if any.
  void stop();

  // Send a file to the master
  void send_file(files::File& file);

  // Send a part of a file to the master
  void send_file_part(files::File& file, size_t part, size_t part_size);
};
