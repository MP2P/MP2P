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
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 0-1 : up_req ~ Upload request <FSIZE|FNAME|RDCY>
      static const what_type up_req_w = 1;
      struct up_req
      {
        fsize_type fsize;
        rdcy_type rdcy;
        //fname_type fname;
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
      static const fromto_type fromto = 1;


      // 1-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 1-1 : stg_table ~ A list of storage's IP <<STID|<IPV6|PORT>>,...>
      static const what_type stg_table_w = 1;
      struct stg_table
      {
        // FIXME (list of STAFIELD)
      } __attribute__ ((packed));


      // 1-2 : pieces_loc ~ The pieces locations <FID<<STID|UINT16>,...>,...>
      static const what_type pieces_loc_w = 2;
      struct pieces_loc
      {
        fid_type fid;
        std::vector<STPFIELD> stps;
      };


      // 1-3 : part_ack ~ Successfully received part acknowledgment <PARTID>
      static const what_type part_ack_w = 3;
      using part_ack = PARTID;
    }
    namespace c_s
    {
      static const fromto_type fromto = 2;


      // 2-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 2-1 : up_act ~ The client sends a piece <PARTID|SHA1|DATA>
      static const what_type up_act_w = 1;
      struct up_act
      {
        PARTID partid;
        sha1_type sha1;
        data_type data;
      } __attribute__ ((packed));


      // 2-2 down_act ~ The client wants a piece <PARTID>
      static const what_type down_act_w = 2;
      struct down_act
      {
        PARTID partid;
      } __attribute__ ((packed));
    }
    namespace s_c
    {
      static const fromto_type from_to = 3;


      // 3-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 3-1 : fail_sha1 ~ Upload failed <PARTID>
      static const what_type fail_sha1_w = 1;
      struct fail_sha1
      {
        PARTID partid;
      } __attribute__ ((packed));


      // 3-2 : up_act ~ The storage sends a piece <PARTID|SHA1|DATA>
      static const what_type up_act_w = 2;
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
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 4-1 del_act : The Master deletes a piece <PARTID>
      static const what_type del_act_w = 1;
      struct del_act
      {
        PARTID partid;
      } __attribute__((packed));


      // 4-2 part_ack : Successfully received part acknowledgment <PARTID>
      static const what_type part_ack_w = 2;
      struct part_ack
      {
        PARTID partid;
      } __attribute__((packed));


      // 4-3 part_loc : Send this part to this storage <PARTID|SHA1|DATA>
      static const what_type part_loc_w = 3;
      struct part_loc
      {
        PARTID partid;
        stid_type stid;
        ADDR addr;
      } __attribute__((packed));
    }
    namespace s_m
    {
      static const fromto_type from_to = 5;


      // 5-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));


      // 5-1 : del_ack ~ Successfully delete part <PARTID>
      static const what_type del_ack_w = 1;
      struct del_ack
      {
        PARTID partid;
      } __attribute((packed));


      // 5-2 : part_ack ~ Successfully received part <PARTID|AVSPACE>
      static const what_type part_ack_w = 2;
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
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));
    }
    namespace s_s
    {
      static const fromto_type from_to = 7;


      // 7-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        err_type err;
      } __attribute__ ((packed));
    }
  }
}
