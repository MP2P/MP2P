#pragma once

namespace utils
{
  inline shared_buffer::shared_buffer(size_t size)
    : data_{std::make_shared<container_type>(size, '\0')},
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

  inline shared_buffer::shared_buffer(const char* data, size_t size)
    : data_{std::make_shared<container_type>(size, '\0')},
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

  inline const shared_buffer::container_type&
  shared_buffer::data_get() const
  {
    return *data_;
  }

  inline const std::string shared_buffer::string_get() const
  {
    return std::string(data_->begin(), data_->end());
  }

  template <typename PointerToPodType>
  PointerToPodType buffer_cast(const shared_buffer& b)
  {
    auto& data = *b.data_;
    return static_cast<PointerToPodType>(&*data.begin());
  }
}
