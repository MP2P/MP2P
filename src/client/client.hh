#pragma once

#include <network.hh>
#include <utils.hh>
#include <experimental/optional>

namespace client
{
  class Client
  {
  private:
    boost::asio::io_service io_service_; // Default constructor is enough
    network::Session master_session_;

    network::error_code recv_handle(network::Packet, network::Session& session);
    network::error_code send_handle(network::Packet, network::Session& session);
    void remove_handle(network::Session& session);

  public:
    Client(const std::string& host, const std::string& port);

    // Run the io service
    void run();

    // Stop the server
    void stop();

    // Send a file to the storage
    void send_file(const files::File& file, network::masks::rdcy_type redundancy);
  };

  // Helpers

  // Send a c_m::up_req to session
  bool request_upload(const files::File& file,
                      network::masks::rdcy_type rdcy,
                      network::Session& session);

  // Send parts to storages
  // In the range [begin_id, end_id)
  void send_parts(const files::File& file,
                  const network::masks::ADDR& addr,
                  size_t begin_id, size_t end_id);
}
