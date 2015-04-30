#pragma once

#include <memory>

namespace utils
{
  class shared_buffer
  {
    public:
      using value_type = boost::asio::mutable_buffer;
      using const_iterator = const boost::asio::mutable_buffer*;

      shared_buffer(size_t size);
      shared_buffer(const std::shared_ptr<std::vector<char>>& data);
      shared_buffer(std::vector<char>&& data);
      shared_buffer(const char* data, size_t size);

      const_iterator begin() const;
      const_iterator end() const;

      const value_type buffer_get() const;
      const std::vector<char>& data_get() const;

      const std::string string_get() const;

    private:
      std::shared_ptr<std::vector<char>> data_;
      boost::asio::mutable_buffer buffer_;
  };
}

#include "shared-buffer.hxx"
