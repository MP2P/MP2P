namespace Database
{
  inline size_t FileItem::nb_part_get() const
  {
    return nb_part_;
  }

  inline size_t FileItem::file_size_get() const
  {
    return file_size_;
  }

  inline size_t FileItem::replication_get() const
  {
    return replication_;
  }

  inline std::string FileItem::uid_get() const
  {
    return uid_;
  }

  inline std::string FileItem ::hash_get() const
  {
    return hash_;
  }

  inline bool FileItem::is_replicated() const
  {
    return replicated_;
  }

  inline bool FileItem::is_uploaded() const
  {
    return uploaded_;
  }

  inline size_t PartItem::size_get() const
  {
    return size_;
  }

  inline std::string PartItem::uid_get() const
  {
    return uid_;
  }

  inline std::string PartItem::hash_get() const
  {
    return hash_;
  }

  inline std::string MasterItem::uid_get() const
  {
    return uid_;
  }

  inline std::string StorageItem::uid_get() const
  {
    return uid_;
  }

  inline size_t StorageItem::available_space_get() const
  {
    return available_space_;
  }
}