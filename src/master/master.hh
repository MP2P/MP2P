#pragma once

#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include <iostream>

#include <network.hh>
#include <utils.hh>
#include "database.hh"

namespace master
{
  class Master
  {
  private:
    boost::asio::io_service io_service_; // Does not need instantiation
    network::Server server_;
    std::vector<std::thread> threads_;

    network::error_code dispatcher(network::Packet packet, network::Session &session);

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

  network::error_code cm_up_req(network::Packet& packet, network::Session& session);
  network::error_code cm_down_req(network::Packet& packet, network::Session& session);
  network::error_code cm_del_req(network::Packet& packet, network::Session& session);
  network::error_code sm_del_ack(network::Packet& packet, network::Session& session);
  network::error_code sm_part_ack(network::Packet& packet, network::Session& session);
}

#include "master.hxx"
#include "master_callbacks.hxx"
