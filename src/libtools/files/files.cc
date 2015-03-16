#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

#include <files.hh>


namespace files
{
  // SHA-1 hash a buffer of bytes
  std::string hash_buffer(const unsigned char* buff, size_t size)
  {
    unsigned char hash[20];
    SHA256(buff, size, hash);
    std::stringstream result;
    for (int i = 0; i < 20; ++i)
      result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return result.str();
  }

}
