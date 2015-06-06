#pragma once

#include <network.hh>
#include <utils.hh>
#include <experimental/optional>
#include <tuple>

namespace client
{
  enum action
  {
    none,
    upload,
    download
  };

  struct Conf
  {
    std::string config_path;
    unsigned concurrency;
    std::string master_hostname;
    network::masks::port_type master_port;
    client::action action;
    std::string file_path;
    network::masks::rdcy_type redundancy;
  };

  extern struct Conf conf;

  void parse_options(int argc, const char *argv[]);

  class Client
  {
  private:
    boost::asio::io_service io_service_; // Default constructor is enough
    network::Session master_session_;

    network::error_code recv_handle(network::Packet, network::Session& session);
    network::error_code send_handle(network::Packet, network::Session& session);
    void remove_handle(network::Session& session);

  public:
    Client(const std::string& host, uint16_t port);

    // Run the io service
    void run();

    // Stop the server
    void stop();

    // Send parts to storages
    // In the range [begin_id, end_id)
    void send_parts(network::masks::fid_type fid,
                    const files::File& file,
                    const network::masks::ADDR& addr,
                    size_t total_parts,
                    size_t begin_id, size_t end_id);

    // Send a c_m::up_req to the master
    void request_upload(const files::File& file,
                        network::masks::rdcy_type rdcy);

    // Send a c_m::down_req to the master
    void request_download(const std::string& filename);
  };

}
