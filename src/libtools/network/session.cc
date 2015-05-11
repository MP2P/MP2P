#include <network.hh>
#include <shared-buffer.hh>


namespace network
{
  using namespace boost::asio;
  using namespace network::masks;

  Session::Session(ip::tcp::socket&& socket,
                   std::function<error_code(Packet,Session&)> dispatcher,
                   std::function<void(Session&)> delete_dispatcher,
                   size_t id)
      : socket_{std::forward<ip::tcp::socket>(socket)},
        dispatcher_{std::move(dispatcher)},
        delete_dispatcher_{std::move(delete_dispatcher)},
        id_{id}
  {
  }

  Session::Session(io_service& io_service,
                   const std::string& host,
                   const std::string& port,
                   std::function<error_code(Packet,Session&)> dispatcher,
                   std::function<void(Session&)> delete_dispatcher,
                   size_t id)
    : socket_{io_service},
      dispatcher_{std::move(dispatcher)},
      delete_dispatcher_{std::move(delete_dispatcher)},
      id_{id}
  {
    ip::tcp::resolver resolver{io_service}; // Resolve the host
    ip::tcp::resolver::query query{host, port};
    ip::tcp::resolver::iterator iter = resolver.resolve(query);
    ip::tcp::endpoint endpoint = *iter;

    boost::system::error_code ec;
    socket_.connect(endpoint, ec); // Connect to the endpoint
    if (ec)
      throw std::logic_error("Unable to connect to server");

    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Opened session (tid=" + s.str() + ")";
  }

  void Session::receive_header(std::function<void(size_t, Packet)> callback)
  {
    async_read(socket_,
        boost::asio::buffer(&*buff_.begin(), buff_.size()),
        transfer_exactly(sizeof (PACKET_HEADER)),
        [this, callback](boost::system::error_code ec, std::size_t size_length)
        {
          if (!ec && size_length == sizeof (PACKET_HEADER))
          {
            const auto* header =
              reinterpret_cast<const PACKET_HEADER*>(buff_.data());

            utils::Logger::cout() << "Receiving a message of size: "
                                     + std::to_string(header->size);

            Packet p{*header}; // Create a packet with the read header
            // Allocate a buffer to read the message into
            p.add_message(empty_message(header->size));

            // Read the whole message + the headers left
            callback(header->size, p);
          }
          else if (ec == boost::asio::error::eof)
            kill();
          else
            utils::Logger::cerr() << "Error while getting size: "
                                  + ec.message();
        }
    );
  }

  void Session::receive_message(size_t msg_size, const Packet& p)
  {
    async_read(socket_,
               p.message_seq_get()[0],
               transfer_exactly(msg_size),
               [this, msg_size, p](boost::system::error_code ec,
                                   std::size_t length)
               {
                 if (!ec)
                 {
                   auto error = dispatcher_(p, *this);
                   if (error == 100)
                     kill();
                   if (length != p.size_get()) // FIXME : This should be tested in the dispatcher
                     receive(); // Keep the socket alive
                 }
                 else
                 {
                   utils::Logger::cerr() << "Error: " + ec.message();
                   kill();
                 }
               }
    );
  }

  // Read on the open socket
  void Session::receive()
  {
    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Session receiving...(tid=" + s.str() + ")";
    receive_header(std::bind(&Session::receive_message,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2));
  }

  // Send a packet on the open socket
  void Session::send(const Packet& packet)
  {
    auto seq = packet.message_seq_get();
    seq.insert(seq.begin(), packet.serialize_header());
    write(socket_, seq);
    auto error = dispatcher_(packet, *this);
    if (error == 1)
      socket_.close();
  }

  size_t Session::unique_id()
  {
    static std::atomic_size_t id;
    return id++;
  }

  void Session::kill()
  {
    utils::Logger::cout() << "Closed session";
    socket_.close(); // Close the socket
    delete_dispatcher_(*this); // Ask the owner to delete
  }
}
