#pragma once

#include <memory>

namespace utils
{
  class shared_const_buffer
  {
    public:
      using value_type = boost::asio::const_buffer;
      using const_iterator = const boost::asio::const_buffer*;

      shared_const_buffer(size_t size);
      shared_const_buffer(const std::shared_ptr<std::vector<char>>& data);
      shared_const_buffer(std::vector<char>&& data);
      shared_const_buffer(unsigned char* data, size_t size);

      const_iterator begin() const;
      const_iterator end() const;

      const value_type buffer_get() const;

    private:
      std::shared_ptr<std::vector<char>> data_;
      boost::asio::const_buffer buffer_;
  };
}

#include "shared-const-buffer.hxx"
