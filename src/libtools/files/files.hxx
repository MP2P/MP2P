#pragma once

#include <files.hh>

namespace files
{

  inline size_t FilePart::size_get() const
  {
    return size_;
  }

  inline const std::string& FilePart::hash_get() const
  {
    return hash_;
  }

  inline const std::string& File::filename_get() const
  {
    return filename_;
  }

  inline std::vector<FilePart>& File::parts_get()
  {
    return parts_;
  }

  inline size_t File::size_get() const
  {
    return size_;
  }

}
