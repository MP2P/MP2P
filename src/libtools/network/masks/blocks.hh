#pragma once

#include <cstdint>
#include <ostream>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <shared-buffer.hh>

namespace network
{
  namespace masks
  {
    struct STAFIELD;
    struct STPFIELD;

    using CharT = char;

    // Constants
    static const size_t ipv6_type_size = 16;
    static const size_t sha1_type_size = 20;

    // Basic blocks
    using avspace_type = uint64_t;
    using err_type = uint16_t;
    using fid_type = uint64_t;
    using fname_type = char*;
    using fromto_type = char;
    using fsize_type = uint64_t;
    using ipv6_type = char[ipv6_type_size];
    using mtid_type = uint32_t;
    using partnum_type = uint32_t;
    using port_type = uint16_t;
    using rdcy_type = uint8_t;
    using sha1_type = char[sha1_type_size];
    using size_type = uint32_t;
    using stid_type = uint32_t;
    using what_type = char;

    using data_type = char*;
    using message_type = utils::shared_buffer;
    using sha1_ptr_type = char*;
    using stplist_type = STPFIELD*;


    // Composed constant size blocks
    struct ADDR
    {
      ipv6_type ipv6;
      port_type port;
    } __attribute__ ((packed));

    struct PARTID
    {
      fid_type fid;
      partnum_type partnum;
    } __attribute__ ((packed));

    struct STPFIELD
    {
      stid_type stid;
      partnum_type nb;
    } __attribute__ ((packed));

    struct TYPE
    {
      fromto_type fromto;
      what_type what;
    } __attribute__ ((packed));


    // Composed variable size blocks
    struct FDETAILS
    {
      fid_type fid;
      stplist_type stplist;
    } __attribute__ ((packed));

    struct PACKET_HEADER
    {
      size_type size;
      TYPE type;
    } __attribute__ ((packed));

    struct STAFIELD
    {
      stid_type stid;
      ADDR addr;
    } __attribute__ ((packed));
  }
}