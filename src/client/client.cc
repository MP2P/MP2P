#include <utils.hh>
#include <files.hh>
#include "client.hh"

namespace client
{

  using namespace network;
  using namespace network::masks;
  using namespace utils;
  using copy = utils::shared_buffer::copy;

  Client::Client(const std::string& host, const std::string& port)
    : master_session_{io_service_, host, port,
        std::bind(&Client::recv_handle, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&Client::send_handle, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&Client::remove_handle, this, std::placeholders::_1)}
  {
  }

  void Client::remove_handle(Session& session)
  {
    (void) session;
  }

  error_code Client::recv_handle(Packet packet, Session& session)
  {
    (void) session;
    (void) packet;

    Logger::cout() << "Client recv handling";

    return 0;
  }

  error_code Client::send_handle(Packet packet, Session& session)
  {
    (void) session;
    (void) packet;

    Logger::cout() << "Client send handling";

    return 0;
  }

  void Client::run()
  {
    io_service_.run();
  }

  void Client::stop()
  {
    // FIXME : Stop everything, join threads if needed
  }

  m_c::pieces_loc request_upload(fsize_type fsize,
                                 rdcy_type rdcy,
                                 std::string fname,
                                 Session& session)
  {
    c_m::up_req request{fsize, rdcy}; // The request message

    Packet req_packet{0, 1};
    req_packet.add_message(reinterpret_cast<CharT*>(&request),
                           sizeof (request),
                           copy::No);
    req_packet.add_message(fname.c_str(), fname.size(), copy::No);

    session.send(req_packet);

    m_c::pieces_loc* pieces; // The expected result

    session.blocking_receive(
        [&pieces](Packet p, Session& /*recv_session*/) -> error_code
        {
          utils::Logger::cout() << p;
          CharT* data = p.message_seq_get().front().data();

          pieces = reinterpret_cast<m_c::pieces_loc*>(data);

          size_t list_size = (p.size_get() - sizeof (fid_type)) / sizeof (STPFIELD);

          for (size_t i = 0; i < list_size; ++i)
          {
            STPFIELD& field = pieces->stps[i];
            std::string ipv6{field.addr.ipv6};
            Logger::cout() << "Field " + std::to_string(i) + " : (" + ipv6
                           + " , " + std::to_string(field.addr.port) + ") , "
                           + std::to_string(field.nb);
          }

          return 0;
        });
    return *pieces; // FIXME : Deep copy, please
  }

  void Client::send_file(files::File& file, masks::rdcy_type redundancy)
  {
    (void)redundancy;
    request_upload(file.size(), redundancy, file.filename_get(), master_session_);

    /*std::vector<std::thread> threads;

    auto size = file.size();
    size_t parts = 4;
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
    */
  }

  void Client::send_file_part(files::File& file, size_t part, size_type part_size)
  {
    const char* tmp = file.data() + part * part_size;

    char pt = part;

    // Example of a packet construction.
    // Add multiple shared_buffers to create a sequence without merging them
    Packet p{2, 1};
    p.add_message(shared_buffer(&pt, sizeof (pt), copy::Yes));
    p.add_message(shared_buffer(tmp, part_size, copy::No));
    send_packet(p);
  }

  void Client::send_packet(const Packet& p)
  {
    // Query needs the port as a string. Ugly fix.
    std::ostringstream port;
    port << Conf::get_instance().port_get();
    const auto& host = Conf::get_instance().host_get();

    Session session{io_service_, host, port.str(),
                    std::bind(&Client::recv_handle, this, std::placeholders::_1,
                              std::placeholders::_2),
                    std::bind(&Client::send_handle, this, std::placeholders::_1,
                              std::placeholders::_2),
                    std::bind(&Client::remove_handle, this,
                              std::placeholders::_1)};
    session.send(p);
  }
}
