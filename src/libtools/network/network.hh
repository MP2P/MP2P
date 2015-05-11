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
#include <masks/messages.hh>

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
    using message_container = std::vector<masks::message_type>;
  private:
    masks::PACKET_HEADER header_;
    message_container message_seq_;

  public:

    // Create a packet with a pointer to data and a size.
    // The data is copied to a shared_buffer
    Packet(masks::size_type size,
           masks::fromto_type fromto,
           masks::what_type what,
           masks::CharT* data);

    Packet(masks::fromto_type fromto,
           masks::what_type what,
           const std::shared_ptr<std::vector<masks::CharT>>& data);

    // Create an empty packet without any message.
    // Use add_message to append messages to the packet
    Packet(const masks::PACKET_HEADER& header);
    Packet(masks::fromto_type fromto,
           masks::what_type what);

    // Create a packet with necessary header fields
    // Append messages as shared_buffers
    template <typename...Messages>
    Packet(masks::fromto_type fromto,
           masks::what_type what,
           Messages...messages);

    // Add a message to the packet. Usually used for sending
    void add_message(const masks::message_type& message);
    void add_message(masks::CharT* data, const masks::size_type size);

    // Add a message to the packet by copying the internal data
    void copy_message(const masks::message_type& message);

    // Accessors
    masks::size_type size_get() const;

    masks::fromto_type fromto_get() const;

    masks::what_type what_get() const;

    message_container& message_seq_get();
    const message_container& message_seq_get() const;

    // Serialize the header of the message to a header to a buffer.
    // It should be added to a sending sequence as well.
    const masks::message_type serialize_header() const;
  };

  masks::message_type empty_message(masks::size_type size);

  Packet deserialize(const masks::PACKET_HEADER header,
                     const masks::message_type& message);

  std::ostream& operator<<(std::ostream& output, const Packet& packet);


  /*-----------.
  | session.cc |
  `-----------*/
  class Session
  {
  private:
    boost::asio::ip::tcp::socket socket_;
    std::array<char, sizeof (masks::PACKET_HEADER)> buff_;
    size_t length_;
    std::function<error_code(Packet, Session&)> dispatcher_;
    std::function<void(Session&)> delete_dispatcher_;
    const size_t id_;

    void receive_header(std::function<void(size_t, Packet)> callback);
    void receive_message(size_t msg_size, const Packet& p);

  public:
    // Create a session
    Session(boost::asio::ip::tcp::socket&& socket,
            std::function<error_code(Packet,Session&)> dispatcher,
            std::function<void(Session&)> delete_dispatcher,
            size_t id = unique_id());

    // Create a session and connect to the host:port
    Session(boost::asio::io_service& io_service,
            const std::string& host,
            const std::string& port,
            std::function<error_code(Packet,Session&)> dispatcher,
            std::function<void(Session&)> delete_dispatcher,
            size_t id = unique_id());

    // Kill the session. Close the socket and remove from parent container
    void kill();

    boost::asio::ip::tcp::socket& socket_get();

    std::array<char, sizeof(masks::PACKET_HEADER)>& buff_get();

    size_t length_get() const;

    size_t id_get() const;

    void receive();

    void send(const Packet& packet);

    void delete_dispatcher(Session& session);

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
    std::function<error_code(Packet,Session&)> dispatcher_;
    std::unordered_map<size_t, Session> sessions_;

  public:
    Server(boost::asio::io_service& io_service,
           std::function<error_code(Packet,Session&)> dispatcher);

    ~Server();

    void listen(); // Listen to accept connections
    void stop();

    bool is_running();

    void delete_dispatcher(Session& session);
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
