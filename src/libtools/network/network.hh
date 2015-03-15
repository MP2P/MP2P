#pragma once

#include <memory>
#include <atomic>
#include <forward_list>
#include <iostream>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
//using namespace boost::asio::ip::tcp;

namespace network
{

  enum class KeepAlive
  {
    ive,
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

    enum ErrorType
    {
      success = 0,
      failure = 1
    };

    Error(const ErrorType et);

    ~Error();

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
    uint32_t size_;
    uint8_t fromto_;
    uint8_t what_;
    std::string message_;

  public:
    Packet(uint8_t fromto, uint8_t what, std::string message);

    ~Packet();

    uint32_t size_get();

    uint8_t fromto_get();

    uint8_t what_get();

    std::string &message_get();

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
    unsigned length_;
    std::function<std::unique_ptr<Error>(Session &)> handler_;
    std::mutex w_mutex_; // Just for testing purposes.

  public:
    Session(ip::tcp::socket &&socket, std::function<std::unique_ptr<Error>(Session &)> handler);

    ip::tcp::socket &socket_get();

    streambuf &buff_get();

    unsigned length_get();

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
    std::function<std::unique_ptr<Error>(Session &)> handler_;
    std::vector<std::shared_ptr<Session>> sessions_;


  public:
    Server(io_service &io_service,
        const unsigned port,
        std::function<std::unique_ptr<Error>(Session &)> handler);

    ~Server();

    boost::asio::streambuf &buff_get();

    void listen(); // Listen to accept connections
    void stop();

    bool is_running();
  };

  std::ostream &operator<<(std::ostream &o, const Error &e);
}
