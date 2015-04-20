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
#include <glob.h>
#include <set>

#include <utils.hh>
#include <files.hh>

namespace network
{
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

    static bool update_conf(const std::string& path);

    ErrorType status_get() const;

    const std::ostringstream& stream_get() const;

    Error& operator=(Error& e);

    Error& operator=(const ErrorType e);

    // Put the parameter in stream_
    template <typename T>
    Error& operator<<(const T& t);

    Error& operator<<(std::ostream& (*f)(std::ostream&));

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
    const uint8_t fromto_;
    const uint8_t what_;
    std::string message_;

  public:
    // Create a packet with a message
    Packet(uint8_t fromto, uint8_t what, std::string message);

    // Create a packet with a pointer to data and a size
    Packet(uint8_t fromto, uint8_t what,
           const char* message, size_t size);

    // Create a packet with a pointer to data and a size
    // Add the hash and the part id
    Packet(uint8_t fromto, uint8_t what,
           const char* message, std::string hash,
           size_t partid, size_t size);

    uint32_t size_get() const;

    uint8_t fromto_get() const;

    uint8_t what_get() const;

    const std::string& message_get() const;

    const std::string serialize() const;
  };

  const Packet deserialize(const std::string& input);

  std::ostream& operator<<(std::ostream& output, const Packet& packet);


  /*-----------.
  | session.cc |
  `-----------*/
  class Session : public std::enable_shared_from_this<Session>
  {
  private:
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buff_;
    size_t length_;
    std::function<error_code(Session&)> handler_;
    std::shared_ptr<std::set<std::shared_ptr<Session>>> parent_container_;

  public:
    // Create a session
    Session(boost::asio::ip::tcp::socket&& socket,
            std::function<error_code(Session& )> handler,
            std::shared_ptr<std::set<std::shared_ptr<Session>>> parent_container);

    // Create a session and connect to the host:port
    Session(boost::asio::io_service& io_service,
            const std::string& host,
            const std::string& port,
            std::function<error_code(Session& )> handler,
            std::shared_ptr<std::set<std::shared_ptr<Session>>> parent_container);

    ~Session();

    boost::asio::ip::tcp::socket& socket_get();

    boost::asio::streambuf& buff_get();

    size_t length_get() const;

    std::string get_line();

    Packet get_packet();

    void receive();

    void send(const Packet& packet);
  };


  /*----------.
  | server.cc |
  `----------*/
  class Server
  {
  private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    std::function<error_code(Session&)> handler_;
    std::set<std::shared_ptr<Session>> sessions_;

  public:
    Server(boost::asio::io_service& io_service,
           std::function<error_code(Session&)> handler);

    ~Server();

    void listen(); // Listen to accept connections
    void stop();

    bool is_running();
  };

  std::ostream& operator<<(std::ostream& o, const Error& e);
}

#include "packet.hxx"
#include "error.hxx"
#include "session.hxx"
