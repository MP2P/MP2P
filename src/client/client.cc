#include <utils.hh>
#include <files.hh>
#include "client.hh"

#include <ostream>

Client::Client()
     : socket_{io_service_}
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

  // FIXME : Check for errors
  boost::system::error_code ec;
  socket_.connect(endpoint, ec); // Connect to the endpoint
}

Client::~Client()
{
  // FIXME : Close sockets and everything if needed
}

std::unique_ptr <Error> Client::handle(Session & session)
{
  (void) session;

  std::cout << "Client handling";

  return std::make_unique<Error>(Error::ErrorType::success);
}

void Client::run()
{
  // Create a shared_ptr to prevent losing the object after exiting the scope
  auto session = std::make_shared<Session>(std::move(socket_),
      std::bind(&Client::handle, this, std::placeholders::_1));

  send(*session); // Ask for a command
  io_service_.run();
}

void Client::send(Session & session)
{
  std::string command;
  std::getline(std::cin, command);
  if (command[command.size() - 1] == '\n')
    command[command.size() - 1] = '\0';

  // Create a file with the file name
  files::File file(command);

  auto& parts = file.parts_get();
  std::ifstream filestream(command);
  for (auto& part : parts)
  {
    filestream.seekg(part.size_get() * part.id_get());
    std::cout << part.size_get() << std::endl;
    Packet p{0, 1, filestream, part.size_get()};
    std::cout << p.message_get() << std::endl;
    session.send(p);
  }
}

void Client::stop()
{
  // FIXME : Stop everything, join threads if needed
}
