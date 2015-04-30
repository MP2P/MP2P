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
#include <glob.h>
#include <unordered_set>
#include <masks/blocks.hh>

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

    static bool initialize(const std::string& path);

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
    PACKET_HEADER header_;
    message_type message_;

  public:
    // Create a packet with all the necessary fields
    Packet(size_type size,
           fromto_type fromto,
           what_type what,
           message_type message);

    // Create a packet with a pointer to data and a size
    Packet(size_type size,
           fromto_type fromto,
           what_type what,
           const char* data);

    Packet(size_type size,
           fromto_type fromto,
           what_type what,
           const std::shared_ptr<std::vector<char>>& data);

    // Create an empty packet with an allocated size
    Packet(const PACKET_HEADER& header);

    // Create a packet with a pointer to data and a size
    // Add the hash and the part id
    Packet(fromto_type fromto, what_type what, const char* message,
                   std::string hash, size_t partid, size_type size);

    size_type size_get() const;

    fromto_type fromto_get() const;

    what_type what_get() const;

    const message_type message_get() const;

    const message_type serialize() const;
  };

  Packet deserialize(const PACKET_HEADER header,
                           const message_type& message);

  std::ostream& operator<<(std::ostream& output, const Packet& packet);


  /*-----------.
  | session.cc |
  `-----------*/
  class Session
  {
  private:
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buff_;
    size_t length_;
    std::function<error_code(Packet, Session&)> handler_;
    std::function<void(Session&)> delete_handler_;
    const size_t id_;

    void receive_header(std::function<void(size_t, Packet)> callback);
    void receive_message(size_t msg_size, Packet p);

  public:
    // Create a session
    Session(boost::asio::ip::tcp::socket&& socket,
            std::function<error_code(Packet,Session&)> handler,
            std::function<void(Session&)> delete_handler,
            size_t id = unique_id());

    // Create a session and connect to the host:port
    Session(boost::asio::io_service& io_service,
            const std::string& host,
            const std::string& port,
            std::function<error_code(Packet,Session&)> handler,
            std::function<void(Session&)> delete_handler,
            size_t id = unique_id());

    // Custom move constructor. Since buff_ doesn't have a move constructor
    Session(Session&& other);

    // Kill the session. Close the socket and remove from parent container
    void kill();

    boost::asio::ip::tcp::socket& socket_get();

    boost::asio::streambuf& buff_get();

    size_t length_get() const;

    size_t id_get() const;

    std::string get_line();

    Packet get_packet();

    void receive();

    void send(const Packet& packet);

    void delete_handler(Session& session);

    static size_t unique_id();
  };

  bool operator==(const Session& lhs, const Session& rhs);

  /*----------.
  | server.cc |
  `----------*/
  class Server
  {
  private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    std::function<error_code(Packet,Session&)> handler_;
    std::unordered_map<size_t, Session> sessions_;

  public:
    Server(boost::asio::io_service& io_service,
           std::function<error_code(Packet,Session&)> handler);

    ~Server();

    void listen(); // Listen to accept connections
    void stop();

    bool is_running();

    void delete_handler(Session& session);
  };

  std::ostream& operator<<(std::ostream& o, const Error& e);
}

#include "packet.hxx"
#include "error.hxx"
#include "session.hxx"
