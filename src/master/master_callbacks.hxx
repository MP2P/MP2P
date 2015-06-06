namespace master
{
  using copy = utils::shared_buffer::copy;

  // May I upload a file?
  inline error_code
  cm_up_req(Packet& packet, Session& session)
  {
    c_m::up_req* req = reinterpret_cast<c_m::up_req*>
        (packet.message_seq_get().front().data());

    //Compute the number of parts.
    uint32_t nb_parts = DB::tools::number_of_parts(req->fsize);
    if (nb_parts == 0)
      return 1;

    std::vector<STPFIELD> fields;
    for (stid_type i = 0; i < nb_parts; ++i)
    {
      STPFIELD field = { get_addr("0:0:0:0:0:0:0:1", 3728), 1 };
      fields.push_back(field);
    }

    // FIXME : Get an unique file id from db
    uint64_t file_id = 1;

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
