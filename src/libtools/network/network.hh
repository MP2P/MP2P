#pragma once

#include <memory>
#include <atomic>
#include <forward_list>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <glob.h>
#include <stdatomic.h>

using namespace boost::asio;
using error_code = uint16_t;

namespace network
{

  enum class KeepAlive
  {
    Live,
    Die
  };

  enum FromTo
  {
    C_to_M = 0,
    M_to_C = 1,
    C_to_S = 2,
    S_to_C = 3,
    M_to_S = 4,
    S_to_M = 5,
    M_to_M = 6,
    S_to_S = 7
  };


  /*---------.
  | error.cc |
  `---------*/
  class Error
  {
  public:
    static std::unordered_map<uint16_t, std::string> errors;

    enum ErrorType
    {
      success = 0,
      failure = 1
    };

    Error(const ErrorType et);

    ~Error();

    static bool update_conf(const std::string &path);

    ErrorType status_get();

    const std::ostringstream &stream_get() const;

    Error &operator=(Error &e);

    Error &operator=(const ErrorType e);

    // Put the parameter in stream_
    template<typename T>
    Error &operator<<(const T &t);

    Error &operator<<(std::ostream &(*f)(std::ostream &));

  private:
    ErrorType status_;
    std::ostringstream stream_;

  };


  /*----------.
  | packet.cc |
  `----------*/
  class Packet
  {
  private:
    unsigned long size_;
    uint8_t fromto_;
    uint8_t what_;
    std::string message_;

  public:
    Packet(uint8_t fromto, uint8_t what, std::string message);

    ~Packet();

    unsigned long get_size();

    uint8_t get_fromto();

    uint8_t get_what();

    std::string &get_message();

    const std::string serialize() const;

    static const Packet deserialize(const std::string &input);
  };

  std::ostream &operator<<(std::ostream &output, const Packet &packet);


  /*-----------.
  | session.cc |
  `-----------*/
  class Session
  {
  private:
    ip::tcp::socket socket_;
    streambuf buff_;
    size_t length_;
    std::function<error_code(Session &)> handler_;
    std::mutex w_mutex_; // Just for testing purposes.

  public:
    Session(ip::tcp::socket &&socket, std::function<error_code(Session &)> handler);

    ip::tcp::socket &socket_get();

    streambuf &buff_get();

    size_t length_get();

    std::string get_line();

    Packet get_packet();

    void receive();

    void send(const Packet packet);
  };


  /*----------.
  | server.cc |
  `----------*/
  class Server
  {
  private:
    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    std::function<error_code(Session &)> handler_;
    std::vector<std::shared_ptr<Session>> sessions_;


  public:
    Server(io_service &io_service,
        std::function<error_code(Session &)> handler);

    ~Server();

    boost::asio::streambuf &buff_get();

    void listen(); // Listen to accept connections
    void stop();

    bool is_running();
  };

  std::ostream &operator<<(std::ostream &o, const Error &e);
}
