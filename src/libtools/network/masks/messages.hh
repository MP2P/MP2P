#pragma once

#include "blocks.hh"

namespace network
{
  namespace masks
  {
    namespace c_m
    {
      static const fromto_type fromto = 0;

      // 0-1 : up_req ~ Upload request <FSIZE|FNAME|RDCY>
      static const what_type up_req_w = 1;
      struct up_req
      {
        fsize_type fsize;
        fname_type fname;
        rdcy_type rdcy;
      } __attribute__ ((packed));

      // 0-2 : down_req ~ Download request <FNAME>
      static const what_type down_req_w = 2;
      struct down_req
      {
        fname_type fname;
      } __attribute__ ((packed));

      // 0-3 : del_req ~ Delete request <FNAME>
      static const what_type del_req_w = 2;
      struct del_req
      {
        fname_type fname;
      } __attribute__ ((packed));
    }
    namespace m_c
    {
      static const fromto_type from_to = 1;
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
