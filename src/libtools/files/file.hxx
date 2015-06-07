#pragma once

#include "files.hh"

#include <boost/filesystem.hpp>

namespace files
{
  inline const std::string& File::filepath_get() const
  {
    return filepath_;
  }

  inline const std::string File::filename_get() const
  {
    boost::filesystem::path p(filepath_);
    return p.filename().string();
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
