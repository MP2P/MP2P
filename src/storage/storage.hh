#pragma once

#include <network.hh>

#include <thread>
#include <vector>
#include <boost/asio.hpp>

namespace storage
{
  /*--------.
  | Options |
  `--------*/

  struct Conf
  {
    std::string config_path;
    std::string hostname;
    network::masks::port_type port;
    unsigned concurrency;
    unsigned timeout;
    std::string master_hostname;
    network::masks::port_type master_port;
    std::string storage_path;
    std::string id_path;
  };

  // Declaration of the storage configuration
  extern struct Conf conf;

  void parse_options(int argc, const char *argv[]);

  /*--------.
  | Storage |
  `---------*/

  class Storage
  {
  public:
    static uint32_t id;
  private:
    boost::asio::io_service io_service_; // Does not need instantiation
    network::Server server_;
    std::vector<std::thread> threads_;

    network::ack_type recv_dispatcher(network::Packet packet,
                                        network::Session& session);
    network::ack_type send_dispatcher(network::Packet packet,
                                        network::Session& session);

    // Causes the server to stop its running threads if any.
    void stop();

    // Request an id from the master, in case the file is missing
    void init_id();

  public:
    Storage();
    ~Storage();

    // Creates threads & make them bind the same port defined in config.
    bool run();

    // Catch a CTRL+C / CTRL+D signal, call Storage::stop(); and exit.
    void catch_stop();

    static uint64_t space_available();
  };

  network::ack_type cs_up_act(network::Packet& packet,
                                network::Session& session);

  network::ack_type cs_down_act(network::Packet& packet,
                                  network::Session& session);

}
