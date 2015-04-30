#pragma once

namespace utils
{
  inline shared_buffer::shared_buffer(size_t size)
    : data_{std::make_shared<std::vector<char>>(size, '\0')},
      buffer_{&*data_->begin(), size}
  {
  }

  inline shared_buffer::shared_buffer(const std::shared_ptr<std::vector<char>>& data)
    : data_{data},
      buffer_{&*data_->begin(), data->size()}
  {
  }

  inline shared_buffer::shared_buffer(std::vector<char>&& data)
    : data_{std::make_shared<std::vector<char>>(
        std::forward<std::vector<char>>(data))},
      buffer_{&*data_->begin(), data.size()}
  {
  }

  inline shared_buffer::shared_buffer(const char* data, size_t size)
    : data_{std::make_shared<std::vector<char>>(size, '\0')},
      buffer_{&*data_->begin(), size}
  {
    memcpy(&*data_->begin(), data, size);
  }

  inline shared_buffer::const_iterator shared_buffer::begin() const
  {
    return &buffer_;
  }

  inline shared_buffer::const_iterator shared_buffer::end() const
  {
    return &buffer_ + 1;
  }

  inline const shared_buffer::value_type
  shared_buffer::buffer_get() const
  {
    return buffer_;
  }

  inline const std::vector<char>& shared_buffer::data_get() const
  {
    return *data_;
  }

  inline const std::string shared_buffer::string_get() const
  {
    return std::string(data_->begin(), data_->end());
  }
}
