#pragma once

#include <network.hh>
#include <utils.hh>

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
    void send_file(files::File& file, network::masks::rdcy_type redundancy);

    // Send a part of a file to the storage
    void send_file_part(files::File& file,
                        size_t part,
                        network::masks::size_type part_size);

    void send_packet(const network::Packet& p);
  };
}
