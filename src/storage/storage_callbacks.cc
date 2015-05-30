#include "storage.hh"

namespace storage
{
  using namespace network;
  using copy = utils::shared_buffer::copy;

  network::error_code cs_up_act(network::Packet& packet, network::Session& session)
  {
    return (packet.size_get() && session.length_get());
  }
}
