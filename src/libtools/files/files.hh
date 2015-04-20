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

      // Accessors
      const std::string& filename_get() const;
      size_t size_get() const;
      const std::string& hash_get() const;
      const char* data() const;

    private:
      const std::string filename_;
      boost::iostreams::mapped_file_source file_;
      std::string hash_;
  };

  // Hash a buffer of chars and return the SHA1 hash as a string
  std::string hash_buffer(const char* buff, size_t size);

  // Get the size of the file
  size_t filesize_get(const std::string& filename);

  // Estimate an optimal number of parts depending of the size of the file
  size_t parts_for_size(size_t size);

  // Get the size of a certain part depending on its size
  // 0 <= part_id < parts_for_size(size)
  size_t part_size_for_size(size_t size, size_t part_id);
}

#include <files.hxx>
