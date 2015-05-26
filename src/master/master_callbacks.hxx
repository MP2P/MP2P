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
    std::cout << "nb_parts = " << nb_parts << std::endl;
    if (nb_parts == 0)
      return 1;

    // Testing purpose
    nb_parts = 4;

    std::vector<STPFIELD> fields;
    for (stid_type i = 0; i < 4; ++i)
    {
      STPFIELD field = {i, 1};
      fields.push_back(field);
    }

    uint64_t file_id = 1;

    Packet response{1, 1};
    response.add_message(reinterpret_cast<const CharT*>(&file_id),
                         sizeof (file_id),
                         copy::Yes);
    response.add_message(reinterpret_cast<const CharT*>(&*fields.begin()),
                         fields.size() * sizeof (STPFIELD),
                         copy::Yes);
    session.send(response);

    //fdetails.stplist = new STPFIELD[nb_storage];
    //For each Storage, select a number of part to send.
    /*for (uint32_t u = 0; u < nb_storage; u++)
    {
      fdetails.stplist[u].stid =;
      fdetails.stplist[u].nb =;
    }*/
    //Packet n(1, 2, fdetails, nb_storage * 6 + 8);
    //session.send(n);
    return (packet.size_get() && session.length_get());
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
    return (packet.size_get() && session.length_get());
  }
}
