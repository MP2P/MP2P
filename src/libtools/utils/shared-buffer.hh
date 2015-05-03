#pragma once

#include <memory>

namespace utils
{
  /* A shared buffer is a buffer containing a vector as a container.
   * It should be used to be copied around and shared between multiple objects
   * and operations.
   */
  class shared_buffer
  {
    public:
      using container_type = std::vector<char>;

      // Empty buffer with a preallocated size
      shared_buffer(size_t size);

      // Buffer with a preallocated container
      shared_buffer(const std::shared_ptr<container_type>& data);

      // Create a buffer by moving a container inside the old one
      shared_buffer(container_type&& data);

      // Copy data from a char buffer to the container
      shared_buffer(const char* data, size_t size);

      // MutableBufferSequence requirements

      using value_type = boost::asio::mutable_buffer;
      using const_iterator = const boost::asio::mutable_buffer*;

      const_iterator begin() const;
      const_iterator end() const;

      // Accessors
      const value_type buffer_get() const;
      const container_type& data_get() const; // FIXME : Don't make this public

      // Create a string using the data
      // Testing purpose only
      const std::string string_get() const;

    private:
      std::shared_ptr<container_type> data_;
      boost::asio::mutable_buffer buffer_;

      template <typename PointerToPodType>
      friend PointerToPodType buffer_cast(const shared_buffer& b);
  };

  template <typename PointerToPodType>
  PointerToPodType buffer_cast(const shared_buffer& b);
}

#include "shared-buffer.hxx"
