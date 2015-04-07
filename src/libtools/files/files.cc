#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <unistd.h>

#include <files.hh>


namespace files
{

  FilePart::FilePart(const size_t size,
                     const size_t id,
                     const std::string& hash)
    : size_(size),
      id_(id),
      hash_(hash)
  {
  }

  FilePart::FilePart(std::ifstream& file,
                     const size_t size,
                     const size_t id)
    : size_(size),
      id_(id)
  {
    hash_compute(file);
  }

  FilePart::FilePart(const std::string& filename,
                     const size_t id)
    : id_(id)
  {
    std::ifstream file(filename, std::ios::binary);
    size_ = filesize_get(file);

    hash_compute(file);
  }

  void FilePart::hash_compute(std::ifstream& file)
  {
    std::string buffer = file_to_buffer(file);
    hash_ = hash_buffer((const unsigned char *)buffer.c_str(), size_);
  }

  File::File(const std::string& filename,
             const std::vector<FilePart>& parts,
             const size_t size)
    : filename_(filename),
      parts_(parts),
      size_(size)
  {
  }

  File::File(const std::string& filename)
    : filename_(filename)
  {
    std::ifstream file(filename);
    if (!file)
      throw std::logic_error("File " + filename + "not found");

    size_ = filesize_get(file);
    auto parts = parts_for_size(size_);
    auto part_size = size_ / parts;
    for (unsigned i = 0; i < parts; ++i)
    {
      file.seekg(part_size * i);
      parts_.emplace_back(file, part_size, i);
    }
  }

  // SHA-1 hash a buffer of bytes
  std::string hash_buffer(const unsigned char* buff, size_t size)
  {
    unsigned char hash[20];
    (void)buff;
    (void)size;
    // FIXME : Activate hashing
    //SHA1(buff, size, hash);
    std::stringstream result;
    for (int i = 0; i < 20; ++i)
      result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return result.str();
  }

  size_t filesize_get(const std::string& filename)
  {
    struct stat st;
    stat(filename.c_str(), &st);
    return st.st_size;
  }

  size_t filesize_get(std::ifstream& open_file)
  {
    // Move the pointer to the end, get the size
    // and put it back where it was

    auto pos = open_file.tellg();
    open_file.seekg(0, std::ios::end);
    size_t size = open_file.tellg();
    open_file.seekg(pos);

    return size;
  }

  std::string file_to_buffer(std::ifstream& file)
  {
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
  }

  std::string read_to_buffer(std::ifstream& file, size_t size)
  {
    std::string buffer(size, '\0');
    file.read(&*buffer.begin(), size);
    return buffer;
  }

  size_t parts_for_size(size_t size)
  {
    // FIXME : Compute the parts relative to the size.
    (void)size;
    return 4;
  }
}
