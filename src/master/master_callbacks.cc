#include "master.hh"
#include <masks/messages.hh>

using namespace network;

// May I upload a file?
error_code cm_up_req(Packet& packet, Session& session)
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

// May I download this file?
error_code cm_down_req(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}

// Can you delete this file?
error_code cm_del_req(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}

// Part deletetion succedeed!
error_code sm_del_ack(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}

// Part successfully received!
error_code sm_part_ack(Packet& packet, Session& session)
{
  return (packet.size_get() && session.length_get());
}
