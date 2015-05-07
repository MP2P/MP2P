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

using namespace network::masks;


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
    using message_container = std::vector<message_type>;
  private:
    PACKET_HEADER header_;
    message_container message_seq_;

  public:
    // Create a packet with necessary header fields
    // Append messages as shared_buffers
    template <typename...Messages>
    Packet(size_type size,
           fromto_type fromto,
           what_type what,
           Messages...messages);

    // Create a packet with a pointer to data and a size.
    // The data is copied to a shared_buffer
    Packet(size_type size,
           fromto_type fromto,
           what_type what,
           CharT* data);

    Packet(size_type size,
           fromto_type fromto,
           what_type what,
           const std::shared_ptr<std::vector<CharT>>& data);

    // Create an empty packet without any message.
    // Use add_message to append messages to the packet
    Packet(const PACKET_HEADER& header);
    Packet(size_type size,
           fromto_type fromto,
           what_type what);

    // Add a message to the packet. Usually used for sending
    void add_message(const message_type& message);

    // Add a message to the packet by copying the internal data
    void copy_message(const message_type& message);

    // Accessors
    size_type size_get() const;

    fromto_type fromto_get() const;

    what_type what_get() const;

    const message_container& message_seq_get() const;

    // Serialize the header of the message to a header to a buffer.
    // It should be added to a sending sequence as well.
    const message_type serialize_header() const;
  };

  message_type empty_message(size_type size);

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
    boost::asio::streambuf buff_; // FIXME : Is this a good choice?
    size_t length_;
    std::function<error_code(Packet, Session&)> handler_;
    std::function<void(Session&)> delete_handler_;
    const size_t id_;

    void receive_header(std::function<void(size_t, Packet)> callback);
    void receive_message(size_t msg_size, const Packet& p);

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

  boost::asio::ip::tcp::resolver::iterator
      resolve_host(const std::string& host);
  boost::asio::ip::address one_ip_from_host(const std::string& host);

}

#include "tools.hxx"
#include "packet.hxx"
#include "error.hxx"
#include "session.hxx"
