#pragma once

#include <network.hh>

#include <future>

namespace client
{
  /*--------.
  | Options |
  `--------*/

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

  /*-------.
  | Client |
  `-------*/

  class Client
  {
  public:
    Client();

    // Send a c_m::up_req to the master
    void request_upload(const files::File& file,
                        network::masks::rdcy_type rdcy);

    // Send a c_m::down_req to the master
    void request_download(const std::string& filename);

  private:
    // One io_service per app
    boost::asio::io_service io_service_;

    // Current asynchronous tasks
    std::vector<std::future<void>> tasks_;

    // Current task progress
    std::atomic<size_t> progress_;

    // End all std::async tasks
    void end_all_tasks();

    // Send parts to storages
    // In the range [begin_id, end_id)
    std::function<void()>
    send_parts(network::masks::fid_type fid,
               const files::File& file,
               const network::masks::ADDR& addr,
               size_t total_parts,
               size_t begin_id, size_t end_id);

    // Recieve a part into file
    std::function<void()>
    recv_part(files::File& file,
              network::masks::ADDR addr,
              network::masks::PARTID partid,
              size_t part_size);
  };
}
