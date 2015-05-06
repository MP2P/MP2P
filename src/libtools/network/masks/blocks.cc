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

  std::string string_from(const char* value, size_t size)
  {
    return "\"" + std::string(value, size) + "\"";
  }
}
