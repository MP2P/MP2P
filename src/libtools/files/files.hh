#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

namespace files
{

  class FilePart
  {
    public:
      /// Construct a part using a size and a hash
      FilePart(const size_t size,
               const std::string& hash);

      /// Construct a part using a filename.
      /// Calculate the hash and the size.
      FilePart(const std::string& filename);

    private:
      size_t size_;
      std::string hash_;

      bool is_hash_correct();
  };

  class File
  {
    public:
      /// Constructor for a normal file
      File(const std::string& filename,
           const std::vector<FilePart>& parts,
           const size_t size);

      /// Constructor for a file that is not split yet.
      File(const std::string& filename,
           const size_t size);

    private:
      std::string filename_;
      std::vector<FilePart> parts_;
      size_t size_;
  };

  /// Hash a buffer of chars and return the SHA1 hash as a string
  std::string hash_buffer(const unsigned char* buff, size_t size);

  /// Get the size of the file
  size_t filesize_get(const std::string& filename);
  /// Get the size of an opened file
  size_t filesize_get(std::ifstream& open_file);

  /// Read a file into a buffer. The buffer allocated enough memory for that.
  std::string file_to_buffer(std::ifstream& file);
}
