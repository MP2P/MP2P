#pragma once

#include "blocks.hh"

namespace network
{
  namespace masks
  {
    // All messages are defined here.
    namespace c_m
    {
      static const fromto_type fromto = 0;


      // 0-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


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
      static const what_type del_req_w = 3;
      struct del_req
      {
        fname_type fname;
      } __attribute__ ((packed));
    }
    namespace m_c
    {
      static const fromto_type from_to = 1;


      // 1-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 1-1 : stg_table ~ A list of storage's IP <<STID|<IPV6|PORT>>,...>
      struct stg_table
      {
        // FIXME (list of STAFIELD)
      } __attribute__ ((packed));


      // 1-2 : pieces_loc ~ The pieces locations <FID<<STID|UINT16>,...>,...>
      struct pieces_loc
      {
        // FIXME (list of FDETAILS)
      } __attribute__ ((packed));


      // 1-3 : part_ack ~ Successfully received part acknowledgment <PARTID>
      using part_ack = PARTID;
    }
    namespace c_s
    {
      static const fromto_type from_to = 2;


      // 2-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 2-1 : up_act ~ The client sends a piece <PARTID|SHA1|DATA>
      struct up_act
      {
        PARTID partid;
        sha1_type sha1;
        data_type data;
      } __attribute__ ((packed));


      // 2-2 down_act ~ The client wants a piece <PARTID>
      struct down_act
      {
        PARTID partid;
      } __attribute__ ((packed));
    }
    namespace s_c
    {
      static const fromto_type from_to = 3;


      // 3-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));

      struct up_act
      {
        PARTID partid;
        sha1_type sha1;
        data_type data;
      } __attribute__ ((packed));
    }
    namespace m_s
    {
      static const fromto_type from_to = 4;


      // 4-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));
    }
    namespace s_m
    {
      static const fromto_type from_to = 5;


      // 5-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));
      struct part_ack
      {
        PARTID partid;
        ipv6_type ip;
        avspace_type avspace;
      } __attribute__ ((packed));
    }
    namespace m_m
    {
      static const fromto_type from_to = 6;


      // 6-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));
    }
    namespace s_s
    {
      static const fromto_type from_to = 7;


      // 7-0 : error ~ Error message <ERR>
      struct error
      {
        err_type err;
      } __attribute__ ((packed));
    }
  }
}
