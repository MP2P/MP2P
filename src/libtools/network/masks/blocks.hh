#pragma once

#include <cstdint>
#include <ostream>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <shared-buffer.hh>

namespace network
{
  struct STAFIELD;
  struct STPFIELD;

  //using err_type = uint16_t;
  using avspace_type = uint64_t;
  using data_type = unsigned char*;
  using fid_type = uint64_t;
  using fname_type = std::string;
  using fromto_type = unsigned char;
  using fsize_type = uint64_t;
  static const size_t ipv6_type_size = 16;
  using ipv6_type = unsigned char[ipv6_type_size];
  using ipv6_return_type = unsigned char*;
  using message_type = boost::asio::mutable_buffer;
  using mtid_type = uint32_t;
  using nb_type = uint16_t; // Number of parts to send to a storage
  using partnum_type = uint32_t;
  using port_type = uint16_t;
  using rdcy_type = uint8_t;
  static const size_t sha1_type_size = 20;
  using sha1_type = unsigned char[sha1_type_size];
  using sha1_return_type = unsigned char*;
  using size_type = uint32_t;
  using stid_type = uint32_t;
  using stalist_type = STAFIELD*;
  using stplist_type = STPFIELD*;
  using what_type = unsigned char;

  struct ADDR
  {
    ipv6_type ipv6;
    port_type port;
  } __attribute__ ((packed));

  struct FDETAILS
  {
    fid_type fid;
    stplist_type stplist;
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
    //char* message; //The message can have a zero size.
  } __attribute__ ((packed));

  struct PARTID
  {
    fid_type fid;
    partnum_type partnum;
  } __attribute__ ((packed));

  struct STAFIELD
  {
    stid_type stid;
    ADDR addr;
  } __attribute__ ((packed));

  struct STPFIELD
  {
    stid_type stid;
    nb_type nb;
  } __attribute__ ((packed));

  template <typename T>
  std::string string_from(T value);

  std::string string_from(const unsigned char* value, size_t size);
}

#include "blocks.hxx"
