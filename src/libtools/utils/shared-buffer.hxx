#pragma once

#include "shared-buffer.hh"

namespace utils
{
  inline shared_buffer::const_iterator
  shared_buffer::begin() const
  {
    return &buffer_;
  }

  inline shared_buffer::const_iterator
  shared_buffer::end() const
  {
    return &buffer_ + 1;
  }

  inline shared_buffer::CharT*
  shared_buffer::data()
  {
    return boost::asio::buffer_cast<shared_buffer::CharT*>(buffer_);
  }

  inline const shared_buffer::CharT*
  shared_buffer::data() const
  {
    return boost::asio::buffer_cast<const shared_buffer::CharT*>(buffer_);
  }

  inline const shared_buffer::value_type
  shared_buffer::buffer_get() const
  {
    return buffer_;
  }

  inline size_t
  shared_buffer::size() const
  {
    return boost::asio::buffer_size(buffer_);
  }

  inline const std::string
  shared_buffer::string_get() const
  {
    // FIXME : Segfaults if data is not copied inside the vector
    return std::string(data_->begin(), data_->end());
  }

  inline shared_buffer::operator
  boost::asio::const_buffer() const
  {
    return boost::asio::const_buffer(buffer_);
  }

  template <typename PointerToPodType>
  PointerToPodType
  buffer_cast(const shared_buffer& b)
  {
    return buffer_cast<PointerToPodType>(b.buffer_);
  }
}
