#include <sys/mman.h>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <cmath>

#include <files.hh>

namespace files
{
  File::File(const std::string& filename)
    : filename_{filename}
  {
    auto size = filesize_get(filename);
    file_ = boost::iostreams::mapped_file_source{filename_, size};
    hash_ = hash_buffer(file_.data(), size_get());
  }

  // SHA-1 hash a buffer of bytes
  std::string hash_buffer(const char* sbuff, size_t size)
  {
    size_t parts = parts_for_size(size);
    size_t part_size = std::ceil((float)size / parts);
    const unsigned char* buff = reinterpret_cast<const unsigned char*>(sbuff);
    unsigned char hash[20];

    // Create a SHA_CTX to accumulate the hash
    // SHA1 function needs the whole buffer to hash it
    SHA_CTX context;
    SHA1_Init(&context);

    // FIXME : maybe use it for sending the hash with the part
    for (size_t i = 0; i < parts; ++i)
    {
      // hash a part, update the final one
      size_t hash_size = part_size_for_size(size, i);
      size_t offset = i * part_size;
      SHA1_Update(&context, buff + offset, hash_size);
    }

    // Get the final hash
    SHA1_Final(hash, &context);

    // Create a string using the hash
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
    // Max authorized file size = 256TB
    <// FIXME : Compute the parts relative to the size.
    (void)size;
    return 4;
  }

  size_t part_size_for_size(size_t size, size_t part_id)
  {
    size_t parts = parts_for_size(size);
    assert(part_id < parts);

    size_t part_size = std::ceil((float)size / parts);
    if (part_id == (parts - 1))
    {
      size_t offset = part_id * part_size;
      if ((offset + part_size) > size)
        part_size -= offset + part_size - size;
    }
    return part_size;
  }
}
