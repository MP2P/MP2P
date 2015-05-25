#pragma once

#include <memory>
#include <boost/asio/buffer.hpp>

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

      // The underlaying container type
      using container_type = std::vector<CharT>;

      // Enum deciding wether data should be copied or not
      enum class copy
      {
        Yes,
        No
      };

      // Construct an empty buffer. It allocates `size` bytes
      explicit shared_buffer(size_t size);

      // Construct a buffer with a preallocated container. No copy occurs
      explicit shared_buffer(const std::shared_ptr<container_type>& data);

      // Construct a buffer by moving a container inside the current one
      explicit shared_buffer(container_type&& data);

      // Construct a buffer by copying (or not) the data from a pointer to POD
      // If your data is going to be invalidated, copy::Yes should be used.
      // If you are sure that your data is going to stay valid,
      // avoid copying using copy::No
      explicit shared_buffer(CharT* data, size_t size, copy to_copy);

      // Same, but using a const buffer.
      explicit shared_buffer(const CharT* data, size_t size, copy to_copy);

      // MutableBufferSequence requirements
      // utils::shared_buffer implements the MutableBufferSequence concept
      // required by boost::asio, in order to be used as a buffer for send/recv
      // functions.

      // FIXME : Something is not right. The const_iterator is mutable.
      //a solution involving templates or traits may be possible.
      using value_type = boost::asio::mutable_buffer;
      using const_iterator = const boost::asio::mutable_buffer*;
      const_iterator begin() const;
      const_iterator end() const;

      // Accessors
      // Raw pointer on the data
      // The pointer is the one coming from the buffer.
      // It may be coming from the vector or from an external data source.
      CharT* data();
      const CharT* data() const;

      // The associated boost::mutable_buffer
      const value_type buffer_get() const;

      // The size of the buffer
      size_t size() const;

      // Create a string using the data
      // Testing purpose only
      const std::string string_get() const;

      // We need this to act as a const_buffer instead of a mutable_buffer
      // for sending data.
      operator boost::asio::const_buffer() const;

    private:
      // The possible underlaying container.
      // Used if copy is needed, or pre-allocated space
      std::shared_ptr<container_type> data_;
      // The mutable_buffer used for interaction with boost::asio
      boost::asio::mutable_buffer buffer_;

      // Copy the data from the pointer to a new allocated buffer
      void copy_helper(const CharT* data, size_t size);

      // Overload buffer_cast for the shared_buffer
      template <typename PointerToPodType>
      friend PointerToPodType buffer_cast(const shared_buffer& b);
  };

  // Overload buffer_cast for the shared_buffer
  template <typename PointerToPodType>
  PointerToPodType buffer_cast(const shared_buffer& b);
}

#include "shared-buffer.hxx"
