#pragma once

#include "files.hh"

namespace files
{
  inline const std::string& File::filename_get() const
  {
    return filename_;
  }

  inline size_t File::size() const
  {
    return file_.size();
  }

  inline char* File::data()
  {
    return file_.data();
  }

  inline const char* File::data() const
  {
    return file_.data();
  }

}
