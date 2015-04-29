#pragma once

namespace utils
{
  shared_const_buffer::shared_const_buffer(size_t size)
    : data_{make_shared<std::vector<char>>(size, '\0')},
      buffer_{&data_->front(), size}
  {
  }

  const_iterator shared_const_buffer::begin()
  {
    return &buffer_;
  }

  const_iterator shared_const_buffer::end()
  {
    return &buffer_ + 1;
  }
}
