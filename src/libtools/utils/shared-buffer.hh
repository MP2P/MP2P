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

      // Construct an empty buffer with a preallocated size
      shared_buffer(size_t size);

      // Construct a uffer with a preallocated container
      shared_buffer(const std::shared_ptr<container_type>& data);

      // Construct a buffer by moving a container inside the current one
      shared_buffer(container_type&& data);

      // Construct a buffer by copying (or not) the data from a pointer to POD
      shared_buffer(char* data, size_t size, bool copy);

      // Construct a buffer by copying (or not) the data from a pointer to POD
      shared_buffer(const char* data, size_t size, bool copy);

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

      operator boost::asio::const_buffer() const;

    private:
      std::shared_ptr<container_type> data_;
      boost::asio::mutable_buffer buffer_;

      void copy_helper(const char* data, size_t size);

      // Overload buffer_cast for the shared_buffer
      template <typename PointerToPodType>
      friend PointerToPodType buffer_cast(const shared_buffer& b);
  };

  // Overload buffer_cast for the shared_buffer
  template <typename PointerToPodType>
  PointerToPodType buffer_cast(const shared_buffer& b);
}

#include "shared-buffer.hxx"
