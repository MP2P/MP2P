#include <boost/filesystem.hpp>

namespace master
{
  using copy = utils::shared_buffer::copy;

  // WARNING: FIXME : This is here for testing purposes.
  // SHOULD BE REPLACED BY THE DATABASE ASAP
  static std::atomic<network::masks::fid_type> id;
  static std::map<std::string, network::masks::fid_type> fids;

  // May I upload a file?
  inline error_code
  cm_up_req(Packet& packet, Session& session)
  {
    c_m::up_req* req = reinterpret_cast<c_m::up_req*>
        (packet.message_seq_get().front().data());

    // FIXME : Get an unique file id from db
    uint64_t file_id = id++;

    fids[std::string(req->fname,
                     packet.size_get() - sizeof (fsize_type) - sizeof (rdcy_type))] = file_id;

    //Compute the number of parts.
    uint32_t nb_parts = DB::tools::number_of_parts(req->fsize);
    if (nb_parts == 0)
      return 1;

    std::vector<STPFIELD> fields;
    for (stid_type i = 0; i < nb_parts; ++i)
    {
      // FIXME : Get storage's ADDR from db
      auto ip = network::get_ipv6("0:0:0:0:0:0:0:1");

      // FIXME : Ugly address initialization
      ADDR addr;
      memcpy(addr.ipv6, ip.to_bytes().data(), ipv6_type_size); // Copy IP
      addr.port = 3728;

      STPFIELD field = { addr, 1 };
      fields.push_back(field);
    }

    Packet response{m_c::fromto, m_c::up_pieces_loc_w};
    response.add_message(reinterpret_cast<const CharT*>(&file_id),
                         sizeof (file_id),
                         copy::Yes);
    response.add_message(reinterpret_cast<const CharT*>(&*fields.begin()),
                         fields.size() * sizeof (STPFIELD),
                         copy::Yes);
    session.send(response);

    return 0;
  }

  // May I download this file?
  inline error_code
  cm_down_req(Packet& packet, Session& session)
  {
    c_m::down_req* req = reinterpret_cast<c_m::down_req*>
        (packet.message_seq_get().front().data());

    std::string fname(req->fname, packet.size_get());

    uint64_t file_id = fids[fname];

    fsize_type fsize = boost::filesystem::file_size(fname);

    utils::Logger::cout() << "Wanna download " + std::to_string(file_id);

    // FIXME : Look into the database for the file.
    // Get the address of each storage that contains the parts

    size_t nb_servers = 1;

    std::vector<STPFIELD> fields;
    for (stid_type i = 0; i < nb_servers; ++i)
    {
      // FIXME : Get storage's ADDR from db
      auto ip = network::get_ipv6("0:0:0:0:0:0:0:1");

      // FIXME : Ugly address initialization
      ADDR addr;
      memcpy(addr.ipv6, ip.to_bytes().data(), ipv6_type_size); // Copy IP
      addr.port = 3728;

      STPFIELD field = { addr, i };
      fields.push_back(field);
    }

    Packet response{m_c::fromto, m_c::down_pieces_loc_w};
    response.add_message(reinterpret_cast<const CharT*>(&fsize),
                         sizeof (fsize_type),
                         copy::Yes);
    response.add_message(reinterpret_cast<const CharT*>(&file_id),
                         sizeof (file_id),
                         copy::Yes);
    response.add_message(reinterpret_cast<const CharT*>(&*fields.begin()),
                         fields.size() * sizeof (STPFIELD),
                         copy::Yes);
    session.send(response);

    return 0;


  }

  // Can you delete this file?
  inline error_code
  cm_del_req(Packet& packet, Session& session)
  {
    return (packet.size_get() && session.length_get());
  }

  // Part deletetion succedeed!
  inline error_code
  sm_del_ack(Packet& packet, Session& session)
  {
    return (packet.size_get() && session.length_get());
  }

  // Part successfully received!
  inline error_code
  sm_part_ack(Packet& packet, Session& session)
  {
    // FIXME : Look in the database if the redundancy is enough.
    // if not, send a request to the storage to upload
    // the file to other storages
    return (packet.size_get() && session.length_get());
  }
}
