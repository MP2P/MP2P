#pragma once

#include <network.hh>
#include <masks/blocks.hh>

namespace network
{
  inline void
  Packet::add_message(const masks::message_type& message)
  {
    message_seq_.push_back(message);
    header_.size += message.size();
  }

  template <typename T>
  inline void
  Packet::add_message(const T* data,
                      const masks::size_type size,
                      utils::shared_buffer::copy to_copy)
  {
    add_message(masks::message_type{
        reinterpret_cast<const masks::CharT*>(data), size, to_copy
    });
  }

  inline masks::size_type
  Packet::size_get() const
  {
    return header_.size;
  }

  inline masks::fromto_type
  Packet::fromto_get() const
  {
    return header_.type.fromto;
  }

  inline masks::what_type
  Packet::what_get() const
  {
    return header_.type.what;
  }

  inline Packet::message_container& Packet::message_seq_get()
  {
    return message_seq_;
  }


  inline const
  Packet::message_container& Packet::message_seq_get() const
  {
    return message_seq_;
  }

  inline masks::message_type
  empty_message(masks::size_type size)
  {
    return utils::shared_buffer(size);
  }

}
