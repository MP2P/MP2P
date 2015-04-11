#pragma once

#include <utils.hh>

namespace utils
{
  // getting the host to bind
  inline std::string Conf::host_get() const
  {
    return host_;
  }

  // getting the port number to bind
  inline unsigned Conf::port_get() const
  {
    return port_;
  }

  // getting the concurrency level
  inline unsigned Conf::concurrency_get() const
  {
    return concurrent_threads_;
  }

  // getting the timeout for server connexions
  inline boost::posix_time::time_duration Conf::timeout_get() const
  {
    return timeout_;
  }
}
