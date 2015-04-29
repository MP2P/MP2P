#pragma once

#include <network.hh>
#include <utils.hh>

class Client
{
private:
  boost::asio::io_service io_service_; // Default constructor is enough
  network::Session master_session_;

  error_code handle(network::Session& session);
  void remove_handle(network::Session& session);

public:
  Client(const std::string& host, const std::string& port);

  // Run the io service
  void run();

  // Stop the server
  void stop();

  // Send a file to the storage
  void send_file(files::File& file);

  // Send a part of a file to the storage
  void send_file_part(files::File& file, size_t part, network::size_type part_size);
};
