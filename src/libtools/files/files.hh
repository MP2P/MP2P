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
               const size_t id,
               const std::string& hash);

      /// Construct a part using an opened stream
      FilePart(std::ifstream& file,
               const size_t id,
               const size_t size);

      /// Construct a part using a filename.
      FilePart(const std::string& filename,
               const size_t id);

      /// Accessors
      size_t size_get() const;
      size_t id_get() const;
      const std::string& hash_get() const;

    private:
      size_t size_;
      size_t id_;
      std::string hash_;

      /// Calculate a hash from a stream
      void hash_compute(std::ifstream& file);
  };

  class File
  {
    public:
      /// Constructor for a normal file
      File(const std::string& filename,
           const std::vector<FilePart>& parts,
           const size_t size);

      /// Constructor for a file that is not split yet.
      File(const std::string& filename);

      /// Accessors
      const std::string& filename_get() const;
      std::vector<FilePart>& parts_get();
      size_t size_get() const;

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

  /// Read a file into a buffer
  std::string file_to_buffer(std::ifstream& file);

  /// Read size amount of bytes from file into a buffer
  std::string read_to_buffer(std::ifstream& file, size_t size);
}

#include <files.hxx>
