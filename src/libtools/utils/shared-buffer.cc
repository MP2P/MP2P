#include "shared-buffer.hh"

namespace utils
{
  shared_buffer::shared_buffer(size_t size)
    : data_{std::make_shared<container_type>(size)},
      buffer_{&*data_->begin(), size}
  {
  }

  shared_buffer::shared_buffer(const std::shared_ptr<container_type>& data)
    : data_{data},
      buffer_{&*data_->begin(), data->size()}
  {
  }

  shared_buffer::shared_buffer(container_type&& data)
    : data_{std::make_shared<container_type>(
        std::forward<container_type>(data))},
      buffer_{&*data_->begin(), data.size()}
  {
  }

  shared_buffer::shared_buffer(char* data, size_t size, copy to_copy)
    : data_{nullptr},
      buffer_{data, size}
  {
    if (to_copy == copy::Yes)
      copy_helper(data, size);
  }

 shared_buffer::shared_buffer(const char* data, size_t size, copy to_copy)
    : data_{nullptr},
      buffer_{const_cast<char*>(data), size}
  {
    if (to_copy == copy::Yes)
      copy_helper(data, size);
  }

  void
  shared_buffer::copy_helper(const char* data, size_t size)
  {
    data_ = std::make_shared<container_type>(size, '\0');
    memcpy(&*data_->begin(), data, size);
    buffer_ = boost::asio::mutable_buffer(&*data_->begin(), size);
  }
}
