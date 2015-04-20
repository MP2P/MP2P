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

  inline std::string Conf::DBhost_get() const
  {
    return DBhost_;
  }

  inline unsigned Conf::DBport_get() const
  {
    return DBport_;
  }

  inline std::string Conf::DBpassword_get() const
  {
    return DBpassword_;
  }

  inline std::string Conf::DBbucket_get() const
  {
    return DBbucket_;
  }
}
