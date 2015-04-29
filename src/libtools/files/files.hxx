#pragma once

#include <files.hh>

namespace files
{
  inline const std::string& File::filename_get() const
  {
    return filename_;
  }

  inline size_t File::size_get() const
  {
    return file_.size();
  }

  inline const std::string& File::hash_get() const
  {
    return hash_;
  }

  inline char* File::data()
  {
    return file_.data();
  }
}
