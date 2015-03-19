#pragma once

#include <forward_list>
#include <thread>
#include <boost/asio.hpp>

#include <network.hh>

using namespace network;
using namespace boost::asio;
using namespace boost::posix_time;

using error_code = uint16_t;

class Master
{
private:
  io_service io_service_; // Does not need instantiation
  network::Server server_;
  std::forward_list<std::thread> threads_;

  error_code handle(Session &session);

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

error_code CM_callback_may_i_upload_a_file(Packet & packet, Session & session);
error_code CM_callback_may_i_download_this_file(Packet & packet, Session & session);
error_code CM_callback_can_you_delete_this_file(Packet & packet, Session & session);
error_code SM_callback_part_deletion_succeded(Packet & packet, Session & session);
error_code SM_callback_part_received(Packet & packet, Session & session);
