#pragma once

#include <forward_list>
#include <thread>
#include <boost/asio.hpp>

#include <network.hh>

class Master
{
private:
  boost::asio::io_service io_service_; // Does not need instantiation
  network::Server server_;
  std::forward_list<std::thread> threads_;

  error_code handle(network::Packet packet, network::Session &session);

  // Causes the server to stop its running threads if any.
  void stop();

public:
  Master();
  ~Master();

  // Creates threads & make them bind the same port defined in config.
  bool run();

  // Catch a CTRL+C / CTRL+D signal, call Master::stop(); and exit.
  void catch_stop();
};

error_code cm_up_req(network::Packet& packet, network::Session& session);
error_code cm_down_req(network::Packet& packet, network::Session& session);
error_code cm_del_req(network::Packet& packet, network::Session& session);
error_code sm_del_ack(network::Packet& packet, network::Session& session);
error_code sm_part_ack(network::Packet& packet, network::Session& session);
