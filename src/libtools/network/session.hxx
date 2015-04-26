#pragma once

#include <network.hh>

namespace network
{
  inline boost::asio::ip::tcp::socket& Session::socket_get()
  {
    return socket_;
  }

  inline boost::asio::streambuf& Session::buff_get()
  {
    return buff_;
  }

  inline size_t Session::length_get() const
  {
    return length_;
  }

  inline size_t Session::id_get() const
  {
    return id_;
  }
}
