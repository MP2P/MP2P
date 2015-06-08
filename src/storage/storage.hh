#pragma once

#include <thread>
#include <vector>
#include <boost/asio.hpp>

#include <network.hh>

namespace storage
{
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

  class Storage
  {
  private:
    boost::asio::io_service io_service_; // Does not need instantiation
    network::Server server_;
    std::vector<std::thread> threads_;

    network::error_code recv_dispatcher(network::Packet packet,
                                        network::Session& session);
    network::error_code send_dispatcher(network::Packet packet,
                                        network::Session& session);

    // Causes the server to stop its running threads if any.
    void stop();

  public:
    Storage();
    ~Storage();

    // Creates threads & make them bind the same port defined in config.
    bool run();

    // Catch a CTRL+C / CTRL+D signal, call Storage::stop(); and exit.
    void catch_stop();
  };

  network::error_code cs_up_act(network::Packet& packet,
                                network::Session& session);

  network::error_code cs_down_act(network::Packet& packet,
                                  network::Session& session);

}
