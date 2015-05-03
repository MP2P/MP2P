#include <utils.hh>
#include <files.hh>
#include "client.hh"

#include <ostream>

using namespace network;
using namespace utils;

Client::Client(const std::string& host, const std::string& port)
  : master_session_{io_service_, host, port,
      std::bind(&Client::handle, this, std::placeholders::_1, std::placeholders::_2),
      std::bind(&Client::remove_handle, this, std::placeholders::_1)}
{
}

void Client::remove_handle(Session& session)
{
  (void) session;
}

error_code Client::handle(Packet packet, Session& session)
{
  (void) session;
  (void) packet;

  Logger::cout() << "Client handling";

  // FIXME : Actually, we should listen after every send.
  // For now, we listen after all the sends, since it's communicating with only
  // one master

  return 0;
}

void Client::run()
{
  io_service_.run();
}

void Client::send_file_part(files::File& file, size_t part, size_type part_size)
{
  // Query needs the port as a string. Ugly fix.
  std::ostringstream port;
  port << Conf::get_instance().port_get();
  const auto& host = Conf::get_instance().host_get();

  Session session{io_service_, host, port.str(),
    std::bind(&Client::handle, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&Client::remove_handle, this, std::placeholders::_1)};

  const char* tmp = file.data() + part * part_size;
  std::string hash = files::hash_buffer(tmp, part_size);

  std::ostringstream ss;
  ss << part << "|" << hash;
  size_type ss_size = ss.str().size();

  // Example of a packet construction.
  // Add multiple shared_buffers to create a sequence without merging them
  Packet p{ss_size + part_size, 0, 1,
           shared_buffer(ss.str().c_str(), ss.str().size(), true),
           shared_buffer(tmp, part_size, false)};
  session.send(p);
}

void Client::send_file(files::File& file)
{
  std::vector<std::thread> threads;

  utils::Logger::cout() << "Sending file with SHA1 hash : " + file.hash_get();

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
    utils::Logger::cout() << "Joining thread";
    thread.join();
  }
  master_session_.receive();
}

void Client::stop()
{
  // FIXME : Stop everything, join threads if needed
}
