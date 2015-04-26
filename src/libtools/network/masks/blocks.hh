#pragma once

#include <cstdint>

struct STAFIELD;
struct STPFIELD;

//using err_type = uint16_t;
using data_type = unsigned char*;
using fid_type = uint64_t;
using fname_type = unsigned char*;
using fromto_type = unsigned char;
using fsize_type = uint64_t;
using ipv6_type = unsigned char[16];
using nb_type = uint16_t;
using partnum_type = uint32_t;
using port_type = uint16_t;
using rdcy_type = uint8_t;
using sha1_type = unsigned char[16];
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

struct PACKET
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
