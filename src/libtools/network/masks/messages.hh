#pragma once

#include "blocks.hh"

namespace network
{
  namespace masks
  {
    namespace c_m
    {
//      extern from_to;
//      from_to = 0;
      using down_req = fname_type;

      struct up_req
      {
        fsize_type fsize;
        fname_type fname;
        rdcy_type rdcy;
      } __attribute__ ((packed));
    }
    namespace c_s
    {
      struct up_act
      {
        PARTID partid;
        sha1_type sha1;
        data_type data;
      } __attribute__ ((packed));
    }
    namespace s_c
    {
      struct up_act
      {
        PARTID partid;
        sha1_type sha1;
        data_type data;
      } __attribute__ ((packed));
    }
    namespace s_m
    {
      struct part_ack
      {
        PARTID partid;
        ipv6_type ip;
        avspace_type avspace;
      } __attribute__ ((packed));
    }
  }
}
