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

  void Client::send_file(const files::File& file, masks::rdcy_type redundancy)
  {
    (void)redundancy;
    request_upload(file, redundancy, master_session_);
  }

  bool request_upload(const files::File& file,
                      rdcy_type rdcy,
                      Session& session)
  {
    fsize_type fsize = file.size();
    const std::string& fname = file.filename_get();

    c_m::up_req request{fsize, rdcy, {}}; // The request message

    Packet req_packet{0, 1};
    req_packet.add_message(reinterpret_cast<CharT*>(&request),
                           sizeof (request),
                           copy::No);
    req_packet.add_message(fname.c_str(), fname.size(), copy::No);

    session.send(req_packet);

    m_c::pieces_loc* pieces; // The expected result

    session.blocking_receive(
        [&pieces, &file](Packet p, Session& /*recv_session*/) -> error_code
        {
          utils::Logger::cout() << p;
          CharT* data = p.message_seq_get().front().data();

          pieces = reinterpret_cast<m_c::pieces_loc*>(data);

          size_t list_size = (p.size_get() - sizeof (fid_type)) / sizeof (STPFIELD);

          // Get total number of parts
          size_t parts = 0;
          for (size_t i = 0; i < list_size; ++i)
            parts += pieces->fdetails.stplist[i].nb;

          Logger::cout() << "Splitting in " + std::to_string(parts) + " parts";

          for (size_t i = 0; i < list_size; ++i)
          {
            STPFIELD& field = pieces->fdetails.stplist[i];
            //std::string ipv6{field.addr.ipv6};
            //Logger::cout() << "Field " + std::to_string(i) + " : (" + ipv6
                           //+ " , " + std::to_string(field.addr.port) + ") , "
                           //+ std::to_string(field.nb);
            send_parts(file, field.addr, parts - field.nb, parts);
            parts -= field.nb;
          }

          return 0;
        });
    return true;
  }

  void send_parts(const files::File& file,
                  const ADDR& addr,
                  size_t begin_id, size_t end_id)
  {
    (void)file;
    std::string ipv6{addr.ipv6};
    Logger::cout() << "Sending "
                   + std::to_string(begin_id) + " - " + std::to_string(end_id)
                   + " to (" + ipv6
                   + " , " + std::to_string(addr.port) + ")";
  }
}
