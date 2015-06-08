#pragma once

#include "blocks.hh"
#include <network.hh>

namespace network
{
  namespace masks
  {

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wzero-length-array"

    // All messages are defined here.
    namespace c_m
    {
      static const fromto_type fromto = 0;

      // 0-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
      } __attribute__ ((packed));


      // 0-1 : up_req ~ Upload request <FSIZE|FNAME|RDCY>
      static const what_type up_req_w = 1;
      struct up_req
      {
        fsize_type fsize;
        rdcy_type rdcy;
        fname_type fname[0];
      } __attribute__ ((packed));


      // 0-2 : down_req ~ Download request <FNAME>
      static const what_type down_req_w = 2;
      struct down_req
      {
        fname_type fname[0];
      } __attribute__ ((packed));


      // 0-3 : del_req ~ Delete request <FNAME>
      static const what_type del_req_w = 3;
      struct del_req
      {
        fname_type fname[0];
      } __attribute__ ((packed));

      // 0-4 . file_ack ~ Check if file has been recieved <FNAME>
      static const what_type file_ack_w = 4;
      struct file_ack
      {
        fname_type fname[0];
      } __attribute__ ((packed));
    }

    namespace m_c
    {
      static const fromto_type fromto = 1;


      // 1-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
      } __attribute__ ((packed));

      // 1-1 : up_pieces_loc ~ The pieces locations <FID<<STID|UINT16>,...>,...>
      static const what_type up_pieces_loc_w = 1;
      struct up_pieces_loc
      {
        FDETAILS fdetails;
      } __attribute__ ((packed));


      // 1-2 : down_pieces_loc ~ The pieces locations <FSIZE<FID<<STID|UINT16>,...>,...>>
      static const what_type down_pieces_loc_w = 2;
      struct down_pieces_loc
      {
        fsize_type fsize;
        FDETAILS fdetails;
      } __attribute__ ((packed));
    }

    namespace c_s
    {
      static const fromto_type fromto = 2;


      // 2-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
      } __attribute__ ((packed));


      // 2-1 : up_act ~ The client sends a piece <PARTID|SHA1|DATA>
      static const what_type up_act_w = 1;
      struct up_act
      {
        PARTID partid;
        sha1_type sha1;
        data_type data[0];
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
      static const fromto_type fromto = 3;


      // 3-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
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
        data_type data[0];
      } __attribute__ ((packed));
    }

    namespace m_s
    {
      static const fromto_type fromto = 4;


      // 4-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
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

      // 4-4 fid_info : Send the STID to the storage <STID>
      static const what_type fid_info_w = 4;
      struct fid_info
      {
        stid_type stid;
      } __attribute__((packed));
    }

    namespace s_m
    {
      static const fromto_type fromto = 5;


      // 5-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
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
        avspace_type avspace;
      } __attribute__ ((packed));

      // 5-3 : id_req ~ Ask for an unique id <PORT>
      static const what_type id_req_w = 3;
      struct id_req
      {
        port_type port;
      } __attribute__ ((packed));
    }

    namespace m_m
    {
      static const fromto_type fromto = 6;


      // 6-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
      } __attribute__ ((packed));
    }

    namespace s_s
    {
      static const fromto_type fromto = 7;


      // 7-0 : error ~ Error message <ERR>
      static const what_type error_w = 0;
      struct error
      {
        network::error_code err;
      } __attribute__ ((packed));


      // 7-1 : up_act ~ Storage sends a piece <PARTID|SHA1|DATA>
      static const what_type up_act_w = 1;
      struct up_act
      {
        PARTID partid;
        sha1_type sha1;
        data_type data[0];
      } __attribute__ ((packed));
    }

    #pragma clang diagnostic pop

  }
}
