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
    using CharT = char;

    // Constants
    static const size_t ipv6_type_size = 16;
    static const size_t sha1_type_size = 20;

    using err_type = uint16_t;

    // Basic blocks
    using avspace_type = uint64_t;
    using fid_type = uint64_t;
    using fname_type = const CharT;
    using fromto_type = uint8_t;
    using fsize_type = uint64_t;
    using ipv6_type = CharT[ipv6_type_size];
    using partnum_type = uint32_t;
    using port_type = uint16_t;
    using rdcy_type = uint8_t;
    using sha1_type = CharT[sha1_type_size];
    using size_type = uint32_t;
    using stid_type = uint32_t;
    using what_type = uint8_t;
    using data_type = const CharT;

    using message_type = utils::shared_buffer;


    // Composed constant size blocks
    struct ADDR
    {
      ipv6_type ipv6;
      port_type port;
    } __attribute__ ((packed));

    struct TYPE
    {
      fromto_type fromto;
      what_type what;
    } __attribute__ ((packed));

    struct PACKET_HEADER
    {
      size_type size;
      TYPE type;
    } __attribute__ ((packed));

    struct PARTID
    {
      fid_type fid;
      partnum_type partnum;
    } __attribute__ ((packed));

    struct STPFIELD
    {
      ADDR addr;
      partnum_type nb; // Part ID or number of parts
    } __attribute__ ((packed));

    //Composed variable size blocks
    struct FDETAILS
    {
      fid_type fid;
      STPFIELD stplist[0];
    } __attribute__ ((packed));

  }
}
