#include "blocks.hh"

using c_m_down_req = fname_type;

struct c_m_up_req
{
  FSIZE fsize;
  fname_type fname;
} __attribute__ ((packed));

struct c_s_up_act
{
  PARTID partid;
  sha1_type sha1;
  data_type data;
} __attribute__ ((packed));

c_s_up_act
c_s_down_act

s_c_fail_sha1
s_c_up_act

m_s_del_act
m_s_part_ack
m_s_part_loc
s_m_del_ack
s_m_part_ack

