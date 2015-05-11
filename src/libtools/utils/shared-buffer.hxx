#pragma once

#include "shared-buffer.hh"

namespace utils
{
  inline shared_buffer::shared_buffer(size_t size)
    : data_{std::make_shared<container_type>(size)},
      buffer_{&*data_->begin(), size}
  {
  }

  inline shared_buffer::shared_buffer(const std::shared_ptr<container_type>& data)
    : data_{data},
      buffer_{&*data_->begin(), data->size()}
  {
  }

  inline shared_buffer::shared_buffer(container_type&& data)
    : data_{std::make_shared<container_type>(
        std::forward<container_type>(data))},
      buffer_{&*data_->begin(), data.size()}
  {
  }

  inline shared_buffer::shared_buffer(char* data, size_t size, bool copy)
    : data_{nullptr},
      buffer_{data, size}
  {
    if (copy)
      copy_helper(data, size);
  }

  inline shared_buffer::shared_buffer(const char* data, size_t size, bool copy)
    : data_{nullptr},
      buffer_{const_cast<char*>(data), size}
  {
    if (copy)
      copy_helper(data, size);
  }

  inline void shared_buffer::copy_helper(const char* data, size_t size)
  {
    data_ = std::make_shared<container_type>(size, '\0');
    memcpy(&*data_->begin(), data, size);
    buffer_ = boost::asio::mutable_buffer(&*data_->begin(), size);
  }

  inline shared_buffer::const_iterator shared_buffer::begin() const
  {
    return &buffer_;
  }

  inline shared_buffer::const_iterator shared_buffer::end() const
  {
    return &buffer_ + 1;
  }

  inline shared_buffer::CharT* shared_buffer::data()
  {
    return &*data_->begin();
  }

  inline const shared_buffer::CharT* shared_buffer::data() const
  {
    return &*data_->begin();
  }

  inline const shared_buffer::value_type
  shared_buffer::buffer_get() const
  {
    return buffer_;
  }

  inline const shared_buffer::container_type&
  shared_buffer::data_get() const
  {
    return *data_;
  }

  inline size_t shared_buffer::size() const
  {
    return data_->size();
  }

  inline const std::string shared_buffer::string_get() const
  {
    return std::string(data_->begin(), data_->end());
  }

  inline shared_buffer::operator boost::asio::const_buffer() const
  {
    return boost::asio::const_buffer(buffer_);
  }

  template <typename PointerToPodType>
  PointerToPodType buffer_cast(const shared_buffer& b)
  {
    auto& data = *b.data_;
    return static_cast<PointerToPodType>(&*data.begin());
  }
}
