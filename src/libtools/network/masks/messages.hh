#pragma once

#include "blocks.hh"

namespace network
{
  using c_m_down_req = fname_type;

  struct c_m_up_req
  {
    fsize_type fsize;
    fname_type fname;
    rdcy_type rdcy;
  } __attribute__ ((packed));

  struct c_s_up_act
  {
    PARTID partid;
    sha1_type sha1;
    data_type data;
  } __attribute__ ((packed));

  struct s_c_up_act
  {
    PARTID partid;
    sha1_type sha1;
    data_type data;
  } __attribute__ ((packed));

  struct s_m_part_ack
  {
    PARTID partid;
    ipv6_type ip;
    avspace_type avspace;
  } __attribute__ ((packed));
}
