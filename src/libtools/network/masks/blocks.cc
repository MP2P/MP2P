#include <string>
#include <sstream>

#include "blocks.hh"

namespace network
{

  template <>
  std::string string_from(std::string value)
  {
    return "\"" + value + "\"";
  }

  template <>
  std::string string_from(uint8_t value)
  {
    std::ostringstream ss;
    ss << unsigned(value);
    return ss.str();
  }

  std::string string_from(const char* value, size_t size)
  {
    return "\"" + std::string(value, size) + "\"";
  }
}
