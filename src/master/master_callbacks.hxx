namespace master
{
  using copy = utils::shared_buffer::copy;

  // May I upload a file?
  inline error_code
  cm_up_req(Packet& packet, Session& session)
  {
    std::cout << "I'm here bitch1" << std::endl;

    c_m::up_req* req = reinterpret_cast<c_m::up_req*>
        (packet.message_seq_get().front().data());

    std::cout << "I'm here bitch2" << std::endl;

    // Check if file already exists
    try
    {
      DB::Connector::get_instance().cmd_get("file." + std::string(req->fname));
      // File already exists: return error
      return 1;
    }
    catch (...)
    { /* Contiinue action */ }

    // Compute the number of parts.
    uint32_t nb_parts = DB::tools::number_of_parts(req->fsize);
    if (nb_parts == 0)
      return 1;

    std::cout << "I'm here bitch3" << std::endl;

    // Create file in DB
    DB::FileItem fi = DB::tools::create_new_file(req->fname,
                                                 req->fsize,
                                                 req->rdcy,
                                                 "");

    std::cout << "I'm here bitch4" << std::endl;

    // Compute STPFIELD(s) depending on file parts.
    std::vector<STPFIELD> fields = DB::tools::get_stpfields_for_upload(fi);

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
