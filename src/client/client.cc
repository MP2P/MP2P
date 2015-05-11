#include <utils.hh>
#include <files.hh>
#include "client.hh"

using namespace network;
using namespace network::masks;
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
  const char* tmp = file.data() + part * part_size;
  std::string hash = files::hash_buffer(tmp, part_size);

  std::ostringstream ss;
  ss << part << "|" << hash;

  // Example of a packet construction.
  // Add multiple shared_buffers to create a sequence without merging them
  Packet p{0, 1,
           shared_buffer(ss.str().c_str(), ss.str().size(), true),
           shared_buffer(tmp, part_size, false)};
  send_packet(p);
}

void Client::send_file(files::File& file)
{
  // c_m_up_req
  c_m_up_req req = c_m_up_req{
      file.size_get(),
      file.filename_get(),
      3
  };
  Packet c_m_up_req_packet = Packet{0, 1};
  c_m_up_req_packet.add_message((CharT*)&req, sizeof(c_m_up_req));

  send_packet(c_m_up_req_packet);



//  std::vector<std::thread> threads;
//
//  utils::Logger::cout() << "Sending file with SHA1 hash : " + file.hash_get();
//
//  auto size = file.size_get();
//  auto parts = files::parts_for_size(size);
//  auto part_size = size / parts;
//  for (size_t i = 0; i < parts; ++i)
//  {
//    threads.emplace_back(
//        [this, &file, i, part_size]()
//        {
//          send_file_part(file, i, part_size);
//        });
//  }
//
//  for (auto& thread : threads)
//  {
//    utils::Logger::cout() << "Joining thread";
//    thread.join();
//  }
//  master_session_.receive();
}

void Client::stop()
{
  // FIXME : Stop everything, join threads if needed
}

void Client::send_packet(Packet& p)
{
  // Query needs the port as a string. Ugly fix.
  std::ostringstream port;
  port << Conf::get_instance().port_get();
  const auto& host = Conf::get_instance().host_get();

  Session session{io_service_, host, port.str(),
                  std::bind(&Client::handle, this, std::placeholders::_1,
                            std::placeholders::_2),
                  std::bind(&Client::remove_handle, this,
                            std::placeholders::_1)};
  std::cout << "sent packet : " << p << std::endl;
  session.send(p);
}