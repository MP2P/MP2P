
#include <files.hh>

namespace files
{

  size_t FilePart::size_get() const
  {
    return size_;
  }

  const std::string& FilePart::hash_get() const
  {
    return hash_;
  }

  const std::string& File::filename_get() const
  {
    return filename_;
  }

  std::vector<FilePart>& File::parts_get()
  {
    return parts_;
  }

  size_t File::size_get() const
  {
    return size_;
  }

}
