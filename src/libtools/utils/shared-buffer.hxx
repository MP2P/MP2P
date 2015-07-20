#pragma once

#include "shared-buffer.hh"
#include "shared-buffer-impl.hh"

namespace utils
{
  inline shared_buffer::const_iterator
  shared_buffer::begin() const
  {
    return &pimpl_->buf_;
  }

  inline shared_buffer::const_iterator
  shared_buffer::end() const
  {
    return &pimpl_->buf_ + 1;
  }

  inline shared_buffer::CharT*
  shared_buffer::data()
  {
    return boost::asio::buffer_cast<shared_buffer::CharT*>(pimpl_->buf_);
  }

  inline const shared_buffer::CharT*
  shared_buffer::data() const
  {
    return boost::asio::buffer_cast<const shared_buffer::CharT*>(pimpl_->buf_);
  }

  inline const shared_buffer::value_type
  shared_buffer::buffer_get() const
  {
    return pimpl_->buf_;
  }

  inline size_t
  shared_buffer::size() const
  {
    return boost::asio::buffer_size(pimpl_->buf_);
  }

  inline const std::string
  shared_buffer::string_get() const
  {
    return std::string(data(), data() + size());
  }

  inline shared_buffer::operator
  boost::asio::const_buffer() const
  {
    return boost::asio::const_buffer(pimpl_->buf_);
  }
}
