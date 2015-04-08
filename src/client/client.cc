#include <utils.hh>
#include <files.hh>
#include "client.hh"

#include <ostream>

Client::Client()
     : master_session_{ boost::asio::ip::tcp::socket(io_service_),
          std::bind(&Client::handle, this, std::placeholders::_1) }
{
  std::cout << "Endpoint host = " << utils::Conf::get_instance().get_host() << std::endl;
  std::cout << "Endpoint port = " << utils::Conf::get_instance().get_port() << std::endl;

  // Query needs the port as a string. Ugly fix.
  std::ostringstream port;
  port << utils::Conf::get_instance().get_port();

  ip::tcp::resolver resolver{io_service_}; // Needed to resolve the host
  ip::tcp::resolver::query query{utils::Conf::get_instance().get_host(), port.str()};

  // Resolve the host and define the endpoint
  ip::tcp::resolver::iterator iter = resolver.resolve(query);
  ip::tcp::endpoint endpoint = *iter;

  boost::system::error_code ec;
  master_session_.socket_get().connect(endpoint, ec); // Connect to the endpoint
  if (ec)
    throw std::logic_error("Unable to connect to server");
}

std::unique_ptr <Error> Client::handle(Session & session)
{
  (void) session;

  std::cout << "Client handling" << std::endl;

  // FIXME : Actually, we should listen after every send.
  // For now, we listen after all the sends, since it's communicating with only
  // one master
//  session.receive();

  return std::make_unique<Error>(Error::ErrorType::success);
}

void Client::run()
{
  io_service_.run();
}

void Client::send_file(files::File& file)
{
  auto size = file.size_get();
  auto parts = files::parts_for_size(size);
  auto part_size = size / parts;
  for (size_t i = 0; i < parts; ++i)
  {
    const char* tmp = file.data() + i * part_size;
    Packet p{0, 1, tmp, part_size};
    master_session_.send(p);
  }
  master_session_.receive();
}

void Client::stop()
{
  // FIXME : Stop everything, join threads if needed
}
