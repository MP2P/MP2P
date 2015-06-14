#pragma once

#include <utils.hh>
#include <network.hh>
#include "database.hh"

#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include <iostream>

using namespace network::masks;

namespace master
{
  struct DB_conf
  {
    std::string hostname;
    std::string password;
    std::string bucket;
  };

  struct Conf
  {
    std::string config_path;
    std::string hostname;
    network::masks::port_type port;
    unsigned concurrency;
    unsigned timeout;
    DB_conf db;
  };

  extern struct Conf conf;

  void parse_options(int argc, const char *argv[]);

  class Master
  {
  private:
    boost::asio::io_service io_service_; // Does not need instantiation
    network::Server server_;
    std::vector<std::thread> threads_;

    network::masks::ack_type recv_dispatcher(network::Packet packet, network::Session &session);
    network::masks::ack_type send_dispatcher(network::Packet packet, network::Session &session);

    // Causes the server to stop its running threads if any.0
    void stop();

  public:
    Master();
    ~Master();

    // Creates threads & make them bind the same port defined in config.
    bool run();

    // Catch a CTRL+C / CTRL+D signal, call Master::stop(); and exit.
    void catch_stop();
  };

  network::masks::ack_type cm_up_req(network::Packet& packet, network::Session& session);
  network::masks::ack_type cm_down_req(network::Packet& packet, network::Session& session);
  network::masks::ack_type cm_del_req(network::Packet& packet, network::Session& session);
  //network::masks::ack_type sm_del_ack(network::Packet& packet, network::Session& session);
  network::masks::ack_type sm_part_ack(network::Packet& packet, network::Session& session);
  network::masks::ack_type sm_id_req(network::Packet& packet, network::Session& session);
}

#include "master.hxx"
#include "master_callbacks.hxx"
#include "option_parser.hxx"
