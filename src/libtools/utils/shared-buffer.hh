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
      // Keep it different from network::masks::CharT.
      // This may be used elsewhere
      using CharT = char;

      using container_type = std::vector<CharT>;

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

      // FIXME : Something is not right. The const_iterator is mutable.
      //a solution involving templates or traits may be possible.
      using value_type = boost::asio::mutable_buffer;
      using const_iterator = const boost::asio::mutable_buffer*;

      const_iterator begin() const;
      const_iterator end() const;

      // Accessors
      // Raw pointer on the data
      CharT* data();
      const CharT* data() const;

      // The associated buffer
      const value_type buffer_get() const;

      // The underlaying container
      const container_type& data_get() const; // FIXME : Don't make this public

      // The size of the buffer
      size_t size() const;

      // Create a string using the data
      // Testing purpose only
      const std::string string_get() const;

      // We need this to act as a const_buffer instead of a mutable_buffer
      // for sending data.
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
