#include <sys/mman.h>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

#include <files.hh>


namespace files
{

  File::File(const std::string& filename)
    : filename_{filename}
  {
    auto size = filesize_get(filename);
    file_ = boost::iostreams::mapped_file_source{filename_, size};
    const unsigned char* to_hash =
      reinterpret_cast<const unsigned char*>(file_.data());
    hash_ = hash_buffer(to_hash, size_get());
  }

  // SHA-1 hash a buffer of bytes
  std::string hash_buffer(const unsigned char* buff, size_t size)
  {
    unsigned char hash[20];
    (void)buff;
    (void)size;
    // FIXME : Activate hashing
    SHA1(buff, size, hash);
    std::stringstream result;
    for (int i = 0; i < 20; ++i)
      result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return result.str();
  }

  size_t filesize_get(const std::string& filename)
  {
    struct stat st;
    stat(filename.c_str(), &st);
    return st.st_size;
  }

  size_t parts_for_size(size_t size)
  {
    // FIXME : Compute the parts relative to the size.
    (void)size;
    return 4;
  }
}
