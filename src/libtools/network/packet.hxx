#pragma once

#include <network.hh>
#include <masks/blocks.hh>

namespace network
{
  template <typename...Messages>
  Packet::Packet(fromto_type fromto,
                 what_type what,
                 Messages...messages)
    : header_{0, {fromto, what} },
      message_seq_{messages...}
  {
    for(auto message = message_seq_.begin(); message != message_seq_.end();
      ++message)
      header_.size += boost::asio::buffer_size(*message);
  }

  inline void
  Packet::add_message(const message_type& message)
  {
    message_seq_.push_back(message);
    // FIXME :
    // header_.size += message.data_get().size();
  }

  inline void
  Packet::add_message(CharT* data, const size_type size)
  {
    header_.size += size;
    message_seq_.push_back(message_type{data, size, true});
  }

  inline size_type
  Packet::size_get() const
  {
    return header_.size;
  }

  inline fromto_type
  Packet::fromto_get() const
  {
    return header_.type.fromto;
  }

  inline what_type
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

  inline message_type
  empty_message(size_type size)
  {
    return utils::shared_buffer(size);
  }

}
