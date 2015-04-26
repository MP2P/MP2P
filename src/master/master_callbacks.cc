#include "master.hh"
#include <masks/messages.hh>

using namespace network;

error_code CM_callback_may_i_upload_a_file(Packet& packet, Session& session)
{
  //TODO: Check if the packet was correct, and send m_c_error if it was not.
  //packet.size_get();

  FDETAILS fdetails;
  //TODO: Assign the fdetails.fid using the database (get the next available id).
  //Compute the number of parts.
  //uint32_t nb_parts = ;
  //Get a list of Storages.
  //
  //uint32_t nb_storage = /*The number of different Storage goes here.*/;
  //fdetails.stplist = new STPFIELD[nb_storage];
  //For each Storage, select a number of part to send.
  /*for (uint32_t u = 0; u < nb_storage; u++)
  {
    fdetails.stplist[u].stid =;
    fdetails.stplist[u].nb =;
  }*/
  //Packet n(1, 2, fdetails, nb_storage * 6 + 8);
  //session.send(n);
  delete fdetails.stplist;

  return (packet.size_get() && session.length_get());
}

error_code CM_callback_may_i_download_this_file(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}

error_code CM_callback_can_you_delete_this_file(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}

error_code SM_callback_part_deletion_succeded(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}

error_code SM_callback_part_received(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}
