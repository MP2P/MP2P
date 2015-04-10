#pragma once

#include <network.hh>

namespace network
{
  inline uint32_t Packet::size_get() const
  {
    return size_;
  }

  inline uint8_t Packet::fromto_get() const
  {
    return fromto_;
  }

  inline uint8_t Packet::what_get() const
  {
    return what_;
  }

  inline const std::string &Packet::message_get() const
  {
    return message_;
  }
}
