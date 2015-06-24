#include "files.hh"

#include <iomanip>
#include <sstream>
#include <cmath>
#include <array>

#include <boost/filesystem.hpp>
#include <openssl/sha.h>

// Apple deprecated OpenSSL and provides its own
// library called CommonCrypto. Use it to avoid warnings on
// deprecated functions.
#ifdef __APPLE__
# define COMMON_DIGEST_FOR_OPENSSL
# include <CommonCrypto/CommonDigest.h>
#endif

namespace files
{
  namespace // Anonymous namespace
  {
    // Get the number of parts for hashing a file
    size_t parts_for_hashing_size(size_t size)
    {
      const size_t mb = 1000000; // 1 MB
      if (size < mb)
        return 1;
      return size / mb;
    }

    // Get the size of a part when hashing a file.
    // Be careful for the last part which might be smaller than the others
    size_t part_size_for_hashing_size(size_t size, size_t part_id)
    {
      size_t parts = parts_for_hashing_size(size);
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

  File::File(const std::string& filepath)
    : filepath_{filepath}
  {
    auto size = boost::filesystem::file_size(filepath);
    file_ = boost::iostreams::mapped_file{filepath_,
                                          std::ios_base::binary
                                          | std::ios_base::in
                                          | std::ios_base::out,
                                          static_cast<unsigned int>(size)};
  }

  File::File(const std::string& filename, size_t size)
    : filepath_{filename}
  {
    boost::iostreams::mapped_file_params params{filename};
    params.flags = boost::iostreams::mapped_file_base::readwrite;
    params.new_file_size = size;

    // Map the file
    file_ = boost::iostreams::mapped_file{params};


    // Fix permissions added by boost::mapped_file.
    // 644 should be the right ones.

    using p = boost::filesystem::perms;
    auto permissions = p::owner_read | p::group_read | p::others_read
                       | p::owner_write;

    // Set the permissions
    boost::filesystem::permissions(filename, permissions);
  }

  File File::create_empty_file(const std::string& filename, size_t size)
  {
    return File{filename, size};
  }

  std::string hash_buffer(const char* sbuff, size_t size)
  {
    auto hash = hash_buffer_hex(sbuff, size);

    // Create a string using the hash
    std::ostringstream result;
    for (int i = 0; i < 20; ++i)
      result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return result.str();
  }

  // SHA-1 hash a buffer of bytes
  std::array<unsigned char, 20> hash_buffer_hex(const char* sbuff, size_t size)
  {
    size_t parts = parts_for_hashing_size(size);
    size_t part_size = std::ceil((float)size / parts);
    const unsigned char* buff = reinterpret_cast<const unsigned char*>(sbuff);
    std::array<unsigned char, 20> hash;

    // Create a SHA_CTX to accumulate the hash
    // SHA1 function needs the whole buffer to hash it
    SHA_CTX context;
    SHA1_Init(&context);

    // FIXME : maybe use it for sending the hash with the part
    for (size_t i = 0; i < parts; ++i)
    {
      // hash a part, update the final one
      size_t hash_size = part_size_for_hashing_size(size, i);
      size_t offset = i * part_size;
      SHA1_Update(&context, buff + offset, hash_size);
    }

    // Get the final hash
    SHA1_Final(hash.data(), &context);

    return hash;
  }

  std::string hash_file(const File& file)
  {
    return hash_buffer(file.data(), boost::filesystem::file_size(file.filepath_get()));
  }

  std::array<unsigned char, 20> hash_file_hex(const File& file)
  {
    return hash_buffer_hex(file.data(), boost::filesystem::file_size(file.filepath_get()));
  }
}
