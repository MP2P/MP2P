#pragma once

#include <thread>
#include <vector>
#include <boost/asio.hpp>

#include <network.hh>

namespace storage
{
  class Storage
  {
  private:
    boost::asio::io_service io_service_; // Does not need instantiation
    network::Server server_;
    std::vector<std::thread> threads_;

    network::error_code dispatcher(network::Packet packet, network::Session& session);

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

  network::error_code CM_callback_may_i_upload_a_file(network::Packet& packet, network::Session& session);
  network::error_code CM_callback_may_i_download_this_file(network::Packet& packet, network::Session& session);
  network::error_code CM_callback_can_you_delete_this_file(network::Packet& packet, network::Session& session);
  network::error_code SM_callback_part_deletion_succeded(network::Packet& packet, network::Session& session);
  network::error_code SM_callback_part_received(network::Packet& packet, network::Session& session);
}
