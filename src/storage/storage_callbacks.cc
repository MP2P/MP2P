#include "storage.hh"
#include <masks/messages.hh>

namespace storage
{
  using namespace network;
  using namespace network::masks;
  using copy = utils::shared_buffer::copy;

  network::error_code cs_up_act(network::Packet& packet, network::Session& session)
  {
    CharT* data = packet.message_seq_get().front().data();
    const c_s::up_act* part = reinterpret_cast<const c_s::up_act*>(data);

    // Save the file to disk
    std::ofstream file(storage::conf.storage_path + '/'
                       + std::to_string(part->partid.fid)
                       + "." + std::to_string(part->partid.partnum));
    file.write(part->data,
               packet.size_get() - sizeof (c_s::up_act));

    auto master_session = create_master_session(session.socket_get().get_io_service());
    Packet p{s_m::fromto, s_m::part_ack_w};
    const s_m::part_ack response{part->partid, 10};
    p.add_message(reinterpret_cast<const CharT*>(&response),
                  sizeof (s_m::part_ack), copy::No);
    master_session.send(p);

    return 0;
  }
}
