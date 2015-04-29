#pragma once

namespace utils
{
  inline shared_const_buffer::shared_const_buffer(size_t size)
    : data_{std::make_shared<std::vector<char>>(size, '\0')},
      buffer_{&*data_->begin(), size}
  {
  }

  inline shared_const_buffer::shared_const_buffer(const std::shared_ptr<std::vector<char>>& data)
    : data_{data},
      buffer_{&*data_->begin(), data->size()}
  {
  }

  inline shared_const_buffer::shared_const_buffer(std::vector<char>&& data)
    : data_{std::make_shared<std::vector<char>>(
        std::forward<std::vector<char>>(data))},
      buffer_{&*data_->begin(), data.size()}
  {
  }

  inline shared_const_buffer::shared_const_buffer(unsigned char* data, size_t size)
    : data_{std::make_shared<std::vector<char>>(size, '\0')},
      buffer_{&*data_->begin(), size}
  {
    memcpy(&*data_->begin(), data, size);
  }

  inline shared_const_buffer::const_iterator shared_const_buffer::begin() const
  {
    return &buffer_;
  }

  inline shared_const_buffer::const_iterator shared_const_buffer::end() const
  {
    return &buffer_ + 1;
  }

  inline const shared_const_buffer::value_type
  shared_const_buffer::buffer_get() const
  {
    return buffer_;
  }
}
