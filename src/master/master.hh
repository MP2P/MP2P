#pragma once

#include <forward_list>
#include <thread>
#include <boost/asio.hpp>
#include <iostream>

#include <network.hh>
#include <utils.hh>
#include "database.hh"


class Master
{
private:
  Database::Database* db_ = nullptr;
  boost::asio::io_service io_service_; // Does not need instantiation
  network::Server server_;
  std::forward_list<std::thread> threads_;

  error_code dispatcher(network::Packet packet, network::Session &session);

  // Causes the server to stop its running threads if any.
  void stop();

public:
  Master(const std::string& host, const std::string& pass,
         const std::string& bucket);
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

#include "master.hxx"
#include "master_callbacks.hxx"