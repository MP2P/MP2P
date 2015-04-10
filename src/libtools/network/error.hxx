#pragma once

#include <network.hh>

namespace network
{
  inline Error::ErrorType Error::status_get() const
  {
    return status_;
  }

  inline const std::ostringstream& Error::stream_get() const
  {
    return stream_;
  }
}
