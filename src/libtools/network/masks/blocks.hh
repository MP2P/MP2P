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
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"

    using CharT = char;

    // Constants
    static const size_t ipv6_type_size = 16;
    static const size_t sha1_type_size = 20;

    // Basic blocks
    using ack_type = uint8_t;
    using avspace_type = uint64_t;
    using data_type = const CharT; //FIXME: No pointer type?
    using fid_type = uint64_t;
    using fname_type = const CharT;
    using fromto_type = uint8_t;
    using fsize_type = uint64_t;
    using ipv6_type = CharT[ipv6_type_size];
    using mtid_type = uint32_t; //FIXME: Useless?
    using partnum_type = uint32_t;
    using partsize_type = uint64_t; //FIXME: What is this?
    using port_type = uint16_t;
    using rdcy_type = uint8_t;
    using sha1_type = unsigned char[sha1_type_size];
    using size_type = uint32_t;
    using stid_type = uint32_t;
    using what_type = uint8_t;

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

    #pragma GCC diagnostic pop
  }
}
