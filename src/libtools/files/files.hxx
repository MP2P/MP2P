#pragma once

#include <files.hh>

namespace files
{

  size_t FilePart::size_get()
  {
    return size_;
  }

  const std::string& FilePart::hash_get()
  {
    return hash_;
  }

  const std::string& File::filename_get()
  {
    return filename_;
  }

  std::vector<FilePart>& File::parts_get()
  {
    return parts_;
  }

  size_t File::size_get()
  {
    return size_;
  }

}
