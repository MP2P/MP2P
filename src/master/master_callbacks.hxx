#include <boost/filesystem.hpp>

namespace master
{
  using copy = utils::shared_buffer::copy;
  using namespace network;

  // May I upload a file?
  inline keep_alive
  cm_up_req(Packet& packet, Session& session)
  {
    const c_m::up_req* req = reinterpret_cast<c_m::up_req*>
        (packet.message_seq_get().front().data());

    std::string fname = std::string(
        req->fname, packet.size_get()
                    - sizeof(req->fsize)
                    - sizeof(req->rdcy)
    );

    auto fi = DB::FileItem(0, fname, req->fsize, req->rdcy, 0,
                           "                    ", 0, {}); // FIXME : Hash me maybe ?

    utils::Logger::cerr() << "Request to upload file " + fi.name_get()
                             + " of size " + std::to_string(fi.file_size_get()) + ".";

    // Compute the number of parts.
    uint32_t nb_parts = DB::tools::number_of_parts(fi.file_size_get());
    if (nb_parts == 0)
      return send_error(session, packet, error_code::missing_storage,
                        "Add some storages so that clients can upload!");

    // Compute STPFIELD(s) depending on file parts.
    std::vector<STPFIELD> fields = DB::tools::get_stpfields_for_upload(fi.file_size_get());

    if (fields.size() < fi.redundancy_get())
      return send_error(session, packet, error_code::redundancy,
                        "Client is asking for a redundancy of "
                         + std::to_string(fi.redundancy_get())
                         + " but there is only "
                         + std::to_string(fields.size())
                         + " storages available.");

    // After all checks, we can now create the file in DB
    DB::tools::create_new_file(fi);

    utils::Logger::cout() << "Parts repartition for " + fi.name_get() + ":";
    for (auto it : fields)
      utils::Logger::cout() << "    -> " + network::binary_to_string_ipv6(it.addr.ipv6, 16) + ":"
                               + std::to_string(it.addr.port)
                               + " count = " + std::to_string(it.nb);

    // Get the unique id of the file
    network::masks::fid_type file_id = fi.id_get();

    Packet response{m_c::fromto, m_c::up_pieces_loc_w};
    response.add_message(&file_id, sizeof (file_id), copy::Yes);
    response.add_message(&*fields.begin(),
                         fields.size() * sizeof (STPFIELD),
                         copy::Yes);

    utils::Logger::cout() << "Responding with m_c::pieces_loc answers for " + fname;
    blocking_send(session.ptr(), response);

    return keep_alive::No;
  }

  // May I download this file?
  inline keep_alive
  cm_down_req(Packet& packet, Session& session)
  {
    const c_m::down_req* req = reinterpret_cast<c_m::down_req*>
        (packet.message_seq_get().front().data());

    std::string fname(req->fname, packet.size_get());

    utils::Logger::cerr() << "Request to download file " + fname + ".";

    std::string json;
    try
    {
      json = DB::Connector::get_instance().cmd_get("file." + fname);
    }
    catch (std::logic_error)
    {
      return send_error(session, packet, error_code::file_not_found,
                        "File " + fname + " does not exist.");
    }

    DB::FileItem fi = DB::FileItem::deserialize(json);

    auto storages = DB::tools::get_all_storages();

    std::vector<STPFIELD> fields;

    // For each part of the file
    for (auto part = fi.parts_get().begin();  part != fi.parts_get().end(); ++part)
    {
      if (part->locations_get().size() == 0)
        return send_error(session, packet, error_code::incomplete_file,
                          "File is not complete on our"
                          "servers (wait for full upload).");

      // For each location of the part
      ADDR addr;
      addr.port = 0;
      for (auto stid : part->locations_get())
      {
        auto st = storages.begin();
        for (; st != storages.end(); ++st)
        {
          if (st->id_get() == stid)
          {
            try
            {
              addr = st->addr_get();
              break;
            }
            catch (...) {}
          }
        }
        if (addr.port != 0)
          break;
      }
      fields.push_back({addr, part->num_get()});
    }

    fsize_type fsize = fi.file_size_get();
    fid_type fid = fi.id_get();

    Packet response{m_c::fromto, m_c::down_pieces_loc_w};
    response.add_message(&fsize, sizeof (fsize_type), copy::Yes);
    response.add_message(&fid, sizeof (fid_type), copy::Yes);
    response.add_message(&*fields.begin(),
                         fields.size() * sizeof (STPFIELD),
                         copy::Yes);
    blocking_send(session.ptr(), response);

    return keep_alive::No;
  }

  // Can you delete this file?
  inline keep_alive
  cm_del_req(Packet& packet, Session& session)
  {
    const c_m::del_req* req = reinterpret_cast<c_m::del_req*>
        (packet.message_seq_get().front().data());

    std::string fname(req->fname, packet.size_get());

    utils::Logger::cerr() << "Request to delete file " + fname + ".";

    std::string json;
    try
    {
      json = DB::Connector::get_instance().cmd_get(fname);
    }
    catch (std::logic_error&)
    {
      return send_error(session, packet, error_code::file_not_found,
                        "File " + fname + " does not exist.");
    }

    DB::FileItem fi = DB::FileItem::deserialize(json);

    auto storages = DB::tools::get_all_storages();

    // For each part of the file
    for (auto part = fi.parts_get().begin();  part != fi.parts_get().end(); ++part)
    {
      if (part->locations_get().size() == 0)
        return send_error(session, packet, error_code::incomplete_file,
                          "File is not complete on our"
                          "servers (wait for full upload).");

      // For each location of the part
      ADDR addr;
      addr.port = 0;
      for (auto stid : part->locations_get())
      {
        auto st = storages.begin();
        for (; st != storages.end(); ++st)
        {
          if (st->id_get() == stid)
          {
            try
            {
              addr = st->addr_get();
              break;
            }
            catch (...) {}
          }
        }
        if (addr.port != 0)
          break;
      }
      //fields.push_back({addr, part->num_get()});
      //FIXME: Where do you send this?
      Packet response{m_s::fromto, m_s::del_act_w};
      //response.add_message(part->num_get(), sizeof (partnum_type), copy::Yes);
      blocking_send(session.ptr(), response);
    }

    return keep_alive::No;
  }


  // FIXME : ...
  static std::mutex mutex_part_ack;

  // Part successfully received!
  inline keep_alive
  sm_part_ack(Packet& packet, Session& session)
  {
    std::lock_guard<std::mutex> lock(mutex_part_ack);
    const s_m::part_ack* req = reinterpret_cast<s_m::part_ack*>
                               (packet.message_seq_get().front().data());

    std::string json = DB::Connector::get_instance().cmd_get("files");
    DB::MetaOnFilesItem mof = DB::MetaOnFilesItem::deserialize(json);
    std::string fname = mof.file_name_by_id(req->partid.fid);
    json = DB::Connector::get_instance().cmd_get("file." + fname);
    DB::FileItem fi = DB::FileItem::deserialize(json);

    auto it = fi.parts_get().begin();
    for (; it != fi.parts_get().end(); ++it)
    {
      if (it->num_get() == req->partid.partnum)
      {
        it->add_stid(req->stid);
      }
    }
    if (it == fi.parts_get().end())
    {
      fi.parts_get().push_back(DB::PartItem(req->partid, "                    ",
                                            {req->stid}));
      it = std::prev(fi.parts_get().end());
    }

    DB::Connector::get_instance().cmd_put("file." + fname, fi.serialize());

    if (it->locations_get().size() >= fi.redundancy_get()) // >= -> Why not?
    {
      send_ack(session, packet, error_code::success);
      return keep_alive::No;
    }
    else // Replication request
    {
      auto storages = DB::tools::get_all_storages();
      std::random_shuffle(storages.begin(),
                          storages.end()); // Good repartition over storages
      for (auto st : storages)
      {
        if (std::find(it->locations_get().begin(), it->locations_get().end(),
                      st.id_get()) != it->locations_get().end())
          continue;
        else
        {
          const m_s::part_loc response{req->partid, st.addr_get()};
          Packet to_send{m_s::fromto, m_s::part_loc_w};
          to_send.add_message(&response, sizeof(m_s::part_loc), copy::Yes);
          blocking_send(session.ptr(), to_send);
          break;
        }
      }
    }

    return keep_alive::No;
  }

  // A new storage poped, and he wants a unique id
  inline keep_alive
  sm_id_req(Packet& packet, Session& session)
  {
    const CharT* data = packet.message_seq_get().front().data();
    const auto* req = reinterpret_cast<const s_m::id_req*>(data);

    // FIXME: CAS this
    std::string json = DB::Connector::get_instance().cmd_get("storages");
    DB::MetaOnStoragesItem mos = DB::MetaOnStoragesItem::deserialize(json);

    // FIXME Get ip of remote storage
    DB::StorageItem si{mos.count_get(), "::1", req->port, req->avspace};
    DB::Connector::get_instance().cmd_put("st." + std::to_string(si.id_get()), si.serialize());

    mos.count_set(mos.count_get() + 1);
    mos.available_space_set(mos.available_space_get() + req->avspace);
    DB::Connector::get_instance().cmd_put("storages", mos.serialize());

    const m_s::fid_info response{si.id_get()};
    Packet to_send{m_s::fromto, m_s::fid_info_w};
    to_send.add_message(&response, sizeof (m_s::fid_info), copy::Yes);
    blocking_send(session.ptr(), to_send);

    return keep_alive::No;
  }
}
