#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/iostreams/device/mapped_file.hpp>

namespace files
{
  class File
  {
    public:

      // Constructor for a file not hashed
      File(const std::string& filename);

      // Disable copying files
      File(const File& other) = delete;
      File& operator=(const File& other) = delete;

      // Explicitly enable move semantics on a file
      File(File&& other) = default;
      File& operator=(File&& other) = default;

      // Accessors
      const std::string& filename_get() const;

      // Get the size from the mapped_file
      size_t size() const;

      // Get a raw pointer on the mapped file data
      char* data();
      const char* data() const;

      // Create an empty file with filename and a size
      static File create_empty_file(const std::string& filename, size_t size);

    private:
      std::string filename_;
      boost::iostreams::mapped_file file_;

      // Private constructor for an empty file.
      // Should be used only for create_empty_file
      File(const std::string& filename, size_t size);
  };

  // Hash a buffer of chars and return the SHA1 hash as a string
  std::string hash_buffer(const char* buff, size_t size);

  // Hash a buffer of chars and return the SHA1 hash as a hex
  std::array<unsigned char, 20> hash_buffer_hex(const char* buff, size_t size);

  // Hash a file. Calling hash_buffer on the mapped area
  std::string hash_file(const File& file);
}

#include "file.hxx"
