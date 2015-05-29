namespace DB
{
  inline fsize_type FileItem::file_size_get() const
  {
    return file_size_;
  }

  inline rdcy_type FileItem::redundancy_get() const
  {
    return redundancy_;
  }

  inline rdcy_type FileItem::current_redundancy_get() const
  {
    return current_redundancy_;
  }

  inline fid_type FileItem::id_get() const
  {
    return id_;
  }

  inline std::string FileItem::name_get() const
  {
    return name_;
  }

  inline sha1_ptr_type FileItem::hash_get()
  {
    return &hash_[0];
  }

  inline bool FileItem::is_replicated() const
  {
    return current_redundancy_ == redundancy_;
  }

  inline bool FileItem::is_uploaded() const
  {
    return uploaded_;
  }

  inline fid_type PartItem::fid_get() const
  {
    return partid_.fid;
  }

  inline partnum_type PartItem::num_get() const
  {
    return partid_.partnum;
  }

  inline sha1_ptr_type PartItem::hash_get()
  {
    return &hash_[0];
  }

  inline std::vector<stid_type> PartItem::locations_get() const
  {
    return locations_;
  }

  inline mtid_type MasterItem::id_get() const
  {
    return id_;
  }

  inline std::string MasterItem::host_addr_get() const
  {
    return host_addr_;
  }

  inline stid_type StorageItem::id_get() const
  {
    return id_;
  }

  inline std::string StorageItem::host_addr_get() const
  {
    return host_addr_;
  }

  inline avspace_type StorageItem::available_space_get() const
  {
    return available_space_;
  }
}
