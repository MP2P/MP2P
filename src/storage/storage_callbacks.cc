#include "storage.hh"
#include <masks/messages.hh>

namespace storage
{
  using namespace network;
  using namespace network::masks;
  using copy = utils::shared_buffer::copy;

  network::error_code cs_up_act(network::Packet& packet, network::Session& /*session*/)
  {
    CharT* data = packet.message_seq_get().front().data();
    const c_s::up_act* part = reinterpret_cast<const c_s::up_act*>(data);

    // Save the file to disk
    std::ofstream file(storage::conf.storage_path + '/'
                       + std::to_string(part->partid.fid)
                       + "." + std::to_string(part->partid.partnum));
    file.write(part->data,
               packet.size_get() - sizeof (c_s::up_act));

    return 0;
  }
}
