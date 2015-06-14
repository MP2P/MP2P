#include "storage.hh"
#include <masks/messages.hh>

namespace storage
{
  using namespace network;
  using namespace network::masks;
  using copy = utils::shared_buffer::copy;

  network::masks::ack_type
  cs_up_act(network::Packet& packet, network::Session& session)
  {
    CharT* data = packet.message_seq_get().front().data();
    const c_s::up_act* part = reinterpret_cast<const c_s::up_act*>(data);

    std::string fid_partnum = "file_id=" + std::to_string(part->partid.fid)
                              + " (part_id=" + std::to_string(part->partid.partnum)
                              + ")";

    utils::Logger::cout() << "Receiving " + fid_partnum + " from "
                             + session.remote_address_get().to_string();

    // Check if the hash is correct
    {
      // Compute the hash of the received buffer
      auto hash = files::hash_buffer_hex(part->data,
                                     packet.size_get() - sizeof (c_s::up_act));
      //FIXME: send a s_c_ack error to the client.
      // If the hash is not correct, send a s_c_fail_sha1 to the client
      // and kill the connection
      if(memcmp(hash.data(), part->sha1, 20))
      {
        utils::Logger::cerr() << "Hash failed for " + fid_partnum;
        //FIXME
        //Packet p{s_c::fromto, s_c::fail_sha1_w};
        Packet p{s_c::fromto, s_c::ack_w};
        p.add_message(&part->partid, sizeof (PARTID), copy::No);
        // FIXME : Use an async_send, but close the session only after the async_send
        session.blocking_send(p);
        return 1;
      }
    }

    // Save the file to disk
    std::ofstream file(storage::conf.storage_path + '/'
                       + std::to_string(part->partid.fid)
                       + "." + std::to_string(part->partid.partnum));

    file.write(part->data, packet.size_get() - sizeof (c_s::up_act));

    auto master_session = Session{session.socket_get().get_io_service(),
                                  conf.master_hostname,
                                  conf.master_port};

    utils::Logger::cout() << "Acknoledging master for " + fid_partnum;
    Packet p{s_m::fromto, s_m::part_ack_w};
    const s_m::part_ack response{Storage::id, part->partid, 10};
    p.add_message(&response, sizeof (s_m::part_ack), copy::Yes);
    master_session.send(p);

    return 0;
  }

  network::masks::ack_type
  cs_down_act(network::Packet& packet, network::Session& session)
  {
    CharT* data = packet.message_seq_get().front().data();
    const c_s::down_act* request = reinterpret_cast<const c_s::down_act*>(data);

    std::ostringstream filename_ss;
    filename_ss << storage::conf.storage_path << '/'
                << request->partid.fid << '.' << request->partid.partnum;

    files::File part{filename_ss.str()};

    auto hash = hash_file_hex(part);

    Packet p{s_c::fromto, s_c::up_act_w};
    // Add the PARTID
    p.add_message(&request->partid, sizeof (PARTID), copy::No);
    // Add the sha1
    p.add_message(hash.data(), hash.size(), copy::No);
    // Add the data
    p.add_message(part.data(), part.size(), copy::No);
    session.blocking_send(p);
    return 0;
  }
}
