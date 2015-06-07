namespace master
{
  using copy = utils::shared_buffer::copy;

  // May I upload a file?
  inline error_code
  cm_up_req(Packet& packet, Session& session)
  {

    c_m::up_req* req = reinterpret_cast<c_m::up_req*>
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

    std::cout << "I'm here bitch3" << std::endl;

    // Create file in DB
    DB::FileItem fi = DB::tools::create_new_file(fname,
                                                 req->fsize,
                                                 req->rdcy,
                                                 "azertyuiopmlkjhgfdsq");

    std::cout << "I'm here bitch4" << std::endl;

    // Compute STPFIELD(s) depending on file parts.
    std::vector<STPFIELD> fields = DB::tools::get_stpfields_for_upload(fi);

    if (fields.size() == 0)
      std::cout << "No srly ?" << std::endl;

    for (auto it : fields)
      std::cout << "Field: " << it.addr.ipv6 << ":" << it.addr.port << " nb=" << it.nb << std::endl;

    // Get the unique id of the file
    network::masks::fid_type file_id = fi.id_get();

    Packet response{m_c::fromto, m_c::pieces_loc_w};
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
    return (packet.size_get() && session.length_get());
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
