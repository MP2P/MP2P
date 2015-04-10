#include "master.hh"

error_code CM_callback_may_i_upload_a_file(Packet & packet, Session & session)
{
  return (packet.size_get() && session.length_get());
}

error_code CM_callback_may_i_download_this_file(Packet & packet, Session & session)
{
  return (packet.size_get() && session.length_get());
}

error_code CM_callback_can_you_delete_this_file(Packet & packet, Session & session)
{
  return (packet.size_get() && session.length_get());
}

error_code SM_callback_part_deletion_succeded(Packet & packet, Session & session)
{
  return (packet.size_get() && session.length_get());
}

error_code SM_callback_part_received(Packet & packet, Session & session)
{
  return (packet.size_get() && session.length_get());
}
