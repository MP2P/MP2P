#pragma once

#include <network.hh>
#include <masks/blocks.hh>

namespace network
{
  inline size_type Packet::size_get() const
  {
    return header_.size;
  }

  inline fromto_type Packet::fromto_get() const
  {
    return header_.type.fromto;
  }

  inline what_type Packet::what_get() const
  {
    return header_.type.what;
  }

  inline const message_type Packet::message_get() const
  {
    return message_;
  }
}
