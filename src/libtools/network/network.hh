#pragma once

#include <masks/blocks.hh>
#include <utils.hh>
#include <files.hh>

#include <memory>
#include <unordered_map>
#include <iosfwd>
#include <boost/asio.hpp>
#include <mutex>

namespace network
{
  /*------.
  | Error |
  `------*/

  // Error codes according to the protocol
  enum class error_code : uint8_t
  {
    success         = 0,  // Success
    error           = 1,  // Unknown error
    file_not_found  = 3,  // File not found
    hash_failed     = 4,  // File hash failed
    invalid_packet  = 5,  // Invalid packet
    missing_storage = 6,
    incomplete_file = 7,
    redundancy      = 11  // Not enough storages
  };

  // Keep the connection alive
  enum class keep_alive
  {
    Yes,
    No
  };

  /*-------.
  | Packet |
  `-------*/

  // Forward declaration
  class Session;

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
    template <typename T>
    void add_message(const T* data,
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

  // FromTo to ToFrom
  masks::fromto_type fromto_inverse(masks::fromto_type fromto);

  // Create an empty message of a precise type.
  masks::message_type empty_message(masks::size_type size);

  // Print a packet's header on an output stream
  std::ostream& operator<<(std::ostream& output, const Packet& packet);

  /*--------.
  | Session |
  `--------*/

  using dispatcher_type = std::function<keep_alive(Packet, Session&)>;

  class Session : public std::enable_shared_from_this<Session>
  {

  public:
    template <typename... Ts>
    static std::shared_ptr<Session> create(Ts&&... params);

  public:
    // Create a session from an actual socket.
    // The dispatcher is called when a packet is transferred.
    // The delete_handler is removing the actual session
    // from the container containing it.
    Session(boost::asio::ip::tcp::socket&& socket,
            dispatcher_type dispatcher,
            size_t id = unique_id());

    // Same with the previous one.
    // The socket parameters are explicitly speciied
    Session(boost::asio::io_service& io_service,
            const std::string& host,
            uint16_t port,
            dispatcher_type dispatcher
              = [](Packet, Session&)
              { return keep_alive::No; },
            size_t id = unique_id());

    // Used for debug
    ~Session();

    // Returns THE shared_ptr associated to the Session
    std::shared_ptr<Session> ptr();

    // Get the corresponding socket
    boost::asio::ip::tcp::socket& socket_get();

    // Get the address of the remote client
    boost::asio::ip::address remote_address_get() const;

    // Get the buffer containing the header
    std::array<char, sizeof(masks::PACKET_HEADER)>& buff_get();

    // Get the dispatcher
    dispatcher_type dispatcher_get() const;

    // The unique session id
    size_t id_get() const;

    // Creates an unique id for a socket.
    // It's using an atomic integer
    static size_t unique_id();

  private:
    // The socket opened for communication
    boost::asio::ip::tcp::socket socket_;

    // The array containing the header of the packet
    // FIXME : Why is it part of the session ?
    std::array<char, sizeof (masks::PACKET_HEADER)> buff_;

    // The dispatcher to call right after a complete recieve
    dispatcher_type dispatcher_;

    // The unique id of the session
    size_t id_;
  };

  // Recieve the header, then call the callback with a packet
  // and a message size
  void receive_header(std::shared_ptr<Session> s,
                      std::function<void(const Packet&,
                                         dispatcher_type)> receive_body,
                      dispatcher_type callback);

  // Recieve the message according to the packet
  void receive_message(std::shared_ptr<Session> s,
                       const Packet& p,
                       dispatcher_type dispatcher);

  // Recieve a header, then the data according to the header
  void receive(std::shared_ptr<Session> s);

  // Receive a header, treat the packet inside a lambda function
  void receive(std::shared_ptr<Session> s, dispatcher_type callback);

  // Same as receive, but blocking
  void blocking_receive(std::shared_ptr<Session> s);

  // Same as receive, but blocking
  void blocking_receive(std::shared_ptr<Session> s, dispatcher_type callback);

  // Send a packet
  void send(std::shared_ptr<Session> s, const Packet& packet);

  // Send a packet
  void send(std::shared_ptr<Session> s, const Packet& packet, dispatcher_type callback);

  // This operation is blocking. It's using a synchronous send
  void blocking_send(std::shared_ptr<Session> s, const Packet& packet);

  // Send a packet using a custom dispatcher
  void blocking_send(std::shared_ptr<Session> s, const Packet& packet, dispatcher_type callback);

  // Compare two Sessions according to their id
  bool operator==(const Session& lhs, const Session& rhs);

  // Send acknowledge (error, or not) to the session according
  // to the packet's header
  void send_ack(Session& session, const Packet& packet, enum error_code ack);

  // Receive acknowledge from the session
  // Throws if any error occurs
  void recv_ack(Session& session);

  /*-------.
  | Server |
  `-------*/

  class Server
  {
  public:
    // Create a server binding addr:port using io_service.
    // Callback dispatcher after a recieve
    Server(boost::asio::ip::address_v6 addr,
           uint16_t port,
           boost::asio::io_service& io_service,
           dispatcher_type dispatcher);

    // Stop the acceptor
    // FIXME : Is this really necessary? What about RAII?
    ~Server();

    // Listen asynchronously for new connections
    void listen();

    // Stop the acceptor
    // FIXME : Is this really necessary? What about RAII?
    void stop();

    bool is_running();

  private:
    // The acceptor used to accept new connections and create sessions
    boost::asio::ip::tcp::acceptor acceptor_;

    // The socket used for listening for new connections
    boost::asio::ip::tcp::socket socket_;

    // The recieve callback
    dispatcher_type dispatcher_;
  };

  /*-----.
  | Misc |
  `-----*/

  boost::asio::ip::tcp::resolver::iterator
      resolve_host(const std::string& host, std::string port = "");
  boost::asio::ip::tcp::resolver::iterator
      resolve_host(const std::string& host, uint16_t port);

  boost::asio::ip::tcp::endpoint endpoint_from_host(const std::string& host,
                                                    const std::string port = "");
  boost::asio::ip::tcp::endpoint endpoint_from_host(const std::string& host,
                                                    uint16_t port);

  boost::asio::ip::address one_ip_from_host(const std::string& host,
                                            const std::string port = "");
  boost::asio::ip::address one_ip_from_host(const std::string& host,
                                            uint16_t port);

  boost::asio::ip::address_v6 get_ipv6(const std::string& str);
  boost::asio::ip::address_v6 get_ipv6(const masks::CharT* ch, size_t size);

  std::string binary_to_string_ipv6(const masks::CharT* ch, size_t size);

  network::masks::ADDR get_addr(const std::string& str,
                                network::masks::port_type port);

  network::masks::partsize_type get_part_size(network::masks::fsize_type fsize,
                                              network::masks::partnum_type partnum,
                                              network::masks::partnum_type parts);
}

#include "misc.hxx"
#include "packet.hxx"
#include "session.hxx"
