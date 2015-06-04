#pragma once

#include <memory>
#include <atomic>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <glob.h>

#include <masks/blocks.hh>
#include <utils.hh>
#include <files.hh>

namespace network
{
  using error_code = uint16_t;

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

//    static bool initialize(const std::string& path);

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

  public:
    using message_container = std::vector<masks::message_type>;

    // Create a packet with a pointer to data and a size.
    // The data is copied to a shared_buffer
    Packet(masks::size_type size,
           masks::fromto_type fromto,
           masks::what_type what,
           const masks::CharT* data);

    // Create a packet with a pointer to data and a size.
    // If to_copy is set to copy::Yes, the content is going to be copied
    // to a new buffer.
    // If to_copy is set to copy::No, make sure the pointer to data is
    // going to stay valid during the usage of the packet
    Packet(masks::size_type size,
           masks::fromto_type fromto,
           masks::what_type what,
           const masks::CharT* data,
           utils::shared_buffer::copy to_copy);

    // Create a packet with a message contained inside a shared_buffer
    Packet(masks::fromto_type fromto,
           masks::what_type what,
           const utils::shared_buffer& message);

    // Create a packet with a message contained inside a shared pointer.
    // The shared_ptr is going to be passed to a shared_buffer.
    Packet(masks::fromto_type fromto,
           masks::what_type what,
           const std::shared_ptr<std::vector<masks::CharT>>& data);

    // Create an empty packet without any message.
    // In order to add new messages, use add_message.
    Packet(const masks::PACKET_HEADER& header);
    Packet(masks::fromto_type fromto,
           masks::what_type what);

    // Add a message to the packet. Usually used for sending
    void add_message(const masks::message_type& message);

    // Add a message to the packet.
    // Set to_copy to copy::Yes if you want the data
    // to be copied to a new buffer.
    // If not, the data should remain valid during the usage of the packet
    void add_message(const masks::CharT* data,
                     const masks::size_type size,
                     utils::shared_buffer::copy to_copy);

    // Add a message to the packet by copying the internal data
    // Equivalent to add_message(message.data(), message.size(), copy::Yes)
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

  private:
    masks::PACKET_HEADER header_;
    message_container message_seq_;
  };

  // Create an empty message of a precise type.
  masks::message_type empty_message(masks::size_type size);

  // Print a packet's header on an output stream
  std::ostream& operator<<(std::ostream& output, const Packet& packet);

  class Session;

  using dispatcher_type = std::function<error_code(Packet, Session&)>;

  /*-----------.
  | session.cc |
  `-----------*/
  class Session
  {

  public:
    // Create a session from an actual socket.
    // The dispatcher is called when a packet is transferred.
    // The delete_handler is removing the actual session
    // from the container containing it.
    Session(boost::asio::ip::tcp::socket&& socket,
            dispatcher_type recv_dispatcher,
            dispatcher_type send_dispatcher,
            std::function<void(Session&)> delete_dispatcher,
            size_t id = unique_id());

    // Same with the previous one.
    // The socket parameters are explicitly speciied
    Session(boost::asio::io_service& io_service,
            const std::string& host,
            uint16_t port,
            dispatcher_type recv_dispatcher,
            dispatcher_type send_dispatcher,
            std::function<void(Session&)> delete_dispatcher,
            size_t id = unique_id());

    // Kill the session. Close the socket and remove from parent container
    void kill();

    // Get the corresponding socket
    boost::asio::ip::tcp::socket& socket_get();

    // Get the buffer containing the header
    std::array<char, sizeof(masks::PACKET_HEADER)>& buff_get();

    // Get the length of the last message
    size_t length_get() const;

    // The unique session id
    size_t id_get() const;

    // Recieve a header, then the data according to the header
    void receive();

    // Receive a header, treat the packet inside a lambda function
    void receive(dispatcher_type callback);

    // Same as receive, but blocking
    void blocking_receive();

    // Same as receive, but blocking
    void blocking_receive(dispatcher_type callback);

    // Send a packet.
    // This operation is blocking. It's using a synchronous send
    void send(const Packet& packet);

    // Send a packet using a custom dispatcher
    void send(const Packet& packet, dispatcher_type callback);

    // Creates an unique id for a socket.
    // It's using an atomic integer
    static size_t unique_id();

  private:
    // The socket opened for communication
    boost::asio::ip::tcp::socket socket_;

    // The array containing the header of the packet
    std::array<char, sizeof (masks::PACKET_HEADER)> buff_;

    // The length of the last received message
    size_t length_;

    // The dispatcher to call right after a complete recieve
    dispatcher_type recv_dispatcher_;

    // The dispatcher to call right after a complete send
    dispatcher_type send_dispatcher_;

    // The function to call to remove this session from the parent container
    std::function<void(Session&)> delete_dispatcher_;

    // The unique id of the session
    const size_t id_;


    // Recieve the header, then call the callback with a packet
    // and a message size
    void receive_header(std::function<void(const Packet&,
                                           dispatcher_type)> receive_body,
                        dispatcher_type callback);

    // Recieve the message according to the packet
    void receive_message(const Packet& p, dispatcher_type dispatcher);
  };

  // Compare two Sessions according to their id
  bool operator==(const Session& lhs, const Session& rhs);

  /*----------.
  | server.cc |
  `----------*/
  class Server
  {
  private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    dispatcher_type recv_dispatcher_;
    dispatcher_type send_dispatcher_;
    std::unordered_map<size_t, Session> sessions_;

  public:
    Server(boost::asio::ip::address_v6 addr, uint16_t port,
           boost::asio::io_service& io_service,
           dispatcher_type recv_dispatcher,
           dispatcher_type send_dispatcher);

    ~Server();

    void listen(); // Listen to accept connections
    void stop();

    bool is_running();

    void delete_dispatcher(Session& session);
  };

  std::ostream& operator<<(std::ostream& o, const Error& e);

  boost::asio::ip::tcp::resolver::iterator
      resolve_host(const std::string& host, std::string port = "");
  boost::asio::ip::tcp::resolver::iterator
      resolve_host(const std::string& host, uint16_t port);
  boost::asio::ip::address one_ip_from_host(const std::string& host);
  boost::asio::ip::address_v6 get_ipv6(const std::string& str);
}

#include "tools.hxx"
#include "packet.hxx"
#include "error.hxx"
#include "session.hxx"
