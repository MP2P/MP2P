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

error_code Client::handle(Session & session)
{
  (void) session;

  std::cout << "Client handling" << std::endl;

  // FIXME : Actually, we should listen after every send.
  // For now, we listen after all the sends, since it's communicating with only
  // one master
//  session.receive();

  return 0;
}

void Client::run()
{
  io_service_.run();
}

void Client::send_file_part(files::File& file, size_t part, size_t part_size)
{

  // Query needs the port as a string. Ugly fix.
  std::ostringstream port;
  port << utils::Conf::get_instance().get_port();

  auto host = utils::Conf::get_instance().get_host();

  network::Session session{io_service_, host, port.str(),
    std::bind(&Client::handle, this, std::placeholders::_1)};

  const char* tmp = file.data() + part * part_size;
  std::string hash = files::hash_buffer(tmp, part_size);
  Packet p{0, 1, tmp, hash, part, part_size};
  session.send(p);
}

void Client::send_file(files::File& file)
{
  std::vector<std::thread> threads;

  std::cout << "Sending file with SHA1 hash : " << file.hash_get() << std::endl;

  auto size = file.size_get();
  auto parts = files::parts_for_size(size);
  auto part_size = size / parts;
  for (size_t i = 0; i < parts; ++i)
  {
    threads.emplace_back(
        [this, &file, i, part_size]()
        {
          send_file_part(file, i, part_size);
        });
  }

  for (auto& thread : threads)
  {
    std::cout << "Joining thread" << std::endl;
    thread.join();
  }
  master_session_.receive();
}

void Client::stop()
{
  // FIXME : Stop everything, join threads if needed
}
