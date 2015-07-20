#include "shared-buffer.hh"

#include <memory>

namespace utils
{
  shared_buffer::shared_buffer(size_t size)
    : pimpl_{std::make_shared<owning_impl>(size)}
  {
  }

  shared_buffer::shared_buffer(container_type&& container)
    : pimpl_{std::make_shared<owning_impl>(std::move(container))}
  {
  }

  shared_buffer::shared_buffer(CharT* data, size_t size, copy to_copy)
  {
    if (to_copy == copy::Yes)
      pimpl_ = std::make_shared<owning_impl>(data, size);
    else
      pimpl_ = std::make_shared<weak_impl>(data, size);
  }

  shared_buffer::shared_buffer(const CharT* data, size_t size, copy to_copy)
  {
    if (to_copy == copy::Yes)
      pimpl_ = std::make_shared<owning_impl>(data, size);
    else
      pimpl_ = std::make_shared<weak_impl>(data, size);
  }
}
