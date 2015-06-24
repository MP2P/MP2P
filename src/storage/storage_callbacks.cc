#include "storage.hh"
#include <masks/messages.hh>

#include <fstream>

namespace storage
{
  using namespace network;
  using namespace network::masks;
  using copy = utils::shared_buffer::copy;

  network::keep_alive
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
      // If the hash is not correct, send an error to the client
      // and kills the connection
      if(memcmp(hash.data(), part->sha1, 20))
        return send_error(session, packet, error_code::hash_failed,
                          "Hash failed for " + fid_partnum);
    }

    // Save the file to disk
    std::ofstream file(storage::conf.storage_path + '/'
                       + std::to_string(part->partid.fid)
                       + "." + std::to_string(part->partid.partnum));

    file.write(part->data, packet.size_get() - sizeof (c_s::up_act));

    // First, ACK the master that the file has been received
    auto master_session = Session{session.socket_get().get_io_service(),
                                  conf.master_hostname,
                                  conf.master_port};

    utils::Logger::cout() << "Acknoledging master for " + fid_partnum;
    Packet p{s_m::fromto, s_m::part_ack_w};
    const s_m::part_ack response{Storage::id, part->partid, 10};
    p.add_message(&response, sizeof (s_m::part_ack), copy::Yes);
    master_session.send(p);

    // Then, ACK the client as well
    send_ack(session, packet, error_code::success);

    return keep_alive::Yes;
  }

  network::keep_alive
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

    utils::Logger::cout() << "[" + std::to_string(session.id_get()) + "] "
                             "Sending part";

    session.blocking_send(p);

    recv_ack(session); // Throws if an error occurs

    return keep_alive::No;
  }
}
