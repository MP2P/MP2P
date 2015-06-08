#include <boost/filesystem.hpp>

namespace master
{
  using copy = utils::shared_buffer::copy;

  // May I upload a file?
  inline error_code
  cm_up_req(Packet& packet, Session& session)
  {

    const c_m::up_req* req = reinterpret_cast<c_m::up_req*>
        (packet.message_seq_get().front().data());

    std::string fname = std::string(
        req->fname, packet.size_get()
                    - sizeof(req->fsize)
                    - sizeof(req->rdcy)
    );

    utils::Logger::cerr() << "Request to upload file " + fname
                             + " of size " + std::to_string(req->fsize) + ".";

    // Compute the number of parts.
    uint32_t nb_parts = DB::tools::number_of_parts(req->fsize);
    if (nb_parts == 0)
      return 1;

    // Create file in DB
    DB::FileItem fi = DB::tools::create_new_file(fname,
                                                 req->fsize,
                                                 req->rdcy,
                                                 "                    ");

    // Compute STPFIELD(s) depending on file parts.
    std::vector<STPFIELD> fields = DB::tools::get_stpfields_for_upload(fi);

    if (fields.size() == 0)
    {
      utils::Logger::cerr() << "Could not find any Storage to send parts to...";
      return 1;
    }

    utils::Logger::cout() << "Parts repartition for " + fname + ":";
    for (auto it : fields)
    {
      utils::Logger::cout() << "    -> " + network::binary_to_string_ipv6(it.addr.ipv6, 16) + ":"
                               + std::to_string(it.addr.port)
                               + " count = " + std::to_string(it.nb);
    }

    // Get the unique id of the file
    network::masks::fid_type file_id = fi.id_get();

    Packet response{m_c::fromto, m_c::up_pieces_loc_w};
    response.add_message(&file_id, sizeof (file_id), copy::Yes);
    response.add_message(&*fields.begin(),
                         fields.size() * sizeof (STPFIELD),
                         copy::Yes);

    utils::Logger::cout() << "Responding with m_c::pieces_loc answers for " + fname;
    session.send(response);

    return 0;
  }

  // May I download this file?
  inline error_code
  cm_down_req(Packet& packet, Session& session)
  {
    const c_m::down_req* req = reinterpret_cast<c_m::down_req*>
        (packet.message_seq_get().front().data());

    std::string fname(req->fname, packet.size_get());

    fsize_type fsize = boost::filesystem::file_size(fname);

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

    // FIXME : Get file_id from db
    network::masks::fid_type file_id = 0; //fi.id_get();

    Packet response{m_c::fromto, m_c::down_pieces_loc_w};
    response.add_message(&fsize, sizeof (fsize_type), copy::Yes);
    response.add_message(&file_id, sizeof (file_id), copy::Yes);
    response.add_message(&*fields.begin(),
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
//    const s_m::part_ack* req = reinterpret_cast<s_m::part_ack*>
//                               (packet.message_seq_get().front().data());
//
//    (void)req->partid.fid;


    // FIXME : Look in the database if the redundancy is enough.
    // if not, send a request to the storage to upload
    // the file to other storages
    return (packet.size_get() && session.length_get());
  }
}
