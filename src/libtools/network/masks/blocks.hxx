#pragma once

namespace network
{

  template <typename T>
  std::string string_from(T value)
  {
    std::ostringstream ss;
    ss << value;
    return ss.str();
  }
}
