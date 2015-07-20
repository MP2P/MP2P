#include "shared-buffer.hh"

#include <memory>

namespace utils
{
  // The abstract structure of the implementation
  // This allows the implementation of the shared_buffer to be chosen
  // at runtime, without having empty containers and null pointers
  // wandering around.
  struct shared_buffer::shared_buffer_impl
  {
    using CharT = shared_buffer::CharT;
    using container_type = shared_buffer::container_type;

    shared_buffer_impl() = default;

    shared_buffer_impl(const boost::asio::mutable_buffer& buf)
      : buf_{buf}
    {
    }

    virtual ~shared_buffer_impl() = default;

    // Mutable buffer used to interact with boost::asio
    boost::asio::mutable_buffer buf_;
  };

  // The owning implementation copies the data to an internal container
  // It's used to keep data alive accross function calls
  struct owning_impl : shared_buffer::shared_buffer_impl
  {
    using super_type = shared_buffer_impl;

    // Copy the data in the container
    owning_impl(const super_type::CharT* data, size_t size)
      : data_{data, data + size}
    {
      buf_ = boost::asio::mutable_buffer{data_.data(), data_.size()};
    }

    owning_impl(size_t size)
      : data_(size, '\0')
    {
      buf_ = boost::asio::mutable_buffer{data_.data(), data_.size()};
    }

    owning_impl(super_type::container_type&& container)
      : data_{std::move(container)}
    {
    }

    owning_impl(size_t size, super_type::CharT default_char)
      : data_(size, default_char)
    {
      buf_ = boost::asio::mutable_buffer{data_.data(), data_.size()};
    }

    // Underlaying container
    container_type data_;
  };

  // The weak implementation gives the ownership to the constructor
  // of the data
  struct weak_impl : shared_buffer::shared_buffer_impl
  {
    using super_type = shared_buffer_impl;

    weak_impl(const super_type::CharT* data, size_t size)
      : super_type{boost::asio::mutable_buffer(const_cast<super_type::CharT*>(data), size)}
    {
    }
  };
}
