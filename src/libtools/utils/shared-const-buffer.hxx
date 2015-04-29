#pragma once

namespace utils
{
  inline shared_const_buffer::shared_const_buffer(size_t size)
    : data_{std::make_shared<std::vector<char>>(size, '\0')},
      buffer_{&data_->front(), size}
  {
  }

  inline shared_const_buffer::const_iterator shared_const_buffer::begin() const
  {
    return &buffer_;
  }

  inline shared_const_buffer::const_iterator shared_const_buffer::end() const
  {
    return &buffer_ + 1;
  }
}
