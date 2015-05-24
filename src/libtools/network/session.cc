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

  void Session::receive_header(std::function<void(const Packet&,
                                                  dispatcher_type)> receive_body,
                               dispatcher_type callback)
  {
    async_read(socket_,
        boost::asio::buffer(&*buff_.begin(), buff_.size()),
        transfer_exactly(sizeof (PACKET_HEADER)),
        [this, callback, receive_body](boost::system::error_code ec,
                                       std::size_t size_length)
        {
          if (!ec && size_length == sizeof (PACKET_HEADER))
          {
            const auto* header =
              reinterpret_cast<const PACKET_HEADER*>(buff_.data());

            utils::Logger::cout() << "Receiving a message of size: "
                                     + std::to_string(header->size);

            // We're not using the constructor of packet with the full header
            // like Packet{*header} because we don't want the size to
            // be specified inside the header yet.
            // Adding the empty message is going to update the size
            Packet p{header->type.fromto, header->type.what};

            // Allocate a buffer to read the message into
            p.add_message(empty_message(header->size));

            // Read the whole message
            receive_body(p, callback);
          }
          else if (ec != boost::asio::error::eof)
          {
            utils::Logger::cerr() << "Error while getting size: "
                                  + ec.message();
            // Kill the session if an error occured
            kill();
          }
        }
    );
  }

  void Session::receive_message(const Packet& p,
                                dispatcher_type callback)
  {
    async_read(socket_,
               p.message_seq_get()[0],
               transfer_exactly(p.size_get()),
               [this, p, callback](boost::system::error_code ec,
                                   std::size_t length)
               {
                 if (!ec)
                 {
                   length_ = length;
                   auto error = callback(p, *this);
                   length_ = 0;
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


  void Session::receive()
  {
    receive(dispatcher_);
  }

  void Session::receive(dispatcher_type callback)
  {
    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Session receiving...(tid=" + s.str() + ")";
    receive_header(std::bind(&Session::receive_message,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2),
                   callback
                  );
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
