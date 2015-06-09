namespace DB
{
  inline
  fsize_type
  FileItem::file_size_get() const
  {
    return file_size_;
  }

  inline
  rdcy_type
  FileItem::redundancy_get() const
  {
    return redundancy_;
  }

  inline
  rdcy_type
  FileItem::current_redundancy_get() const
  {
    return current_redundancy_;
  }

  inline
  fid_type
  FileItem::id_get() const
  {
    return id_;
  }

  inline
  std::string
  FileItem::name_get() const
  {
    return name_;
  }

  inline
  unsigned char*
  FileItem::hash_get()
  {
    return &hash_[0];
  }

  inline
  bool
  FileItem::is_replicated() const
  {
    return current_redundancy_ == redundancy_;
  }

  inline
  bool
  FileItem::is_uploaded() const
  {
    return uploaded_;
  }

  inline
  std::vector<PartItem>&
  FileItem::parts_get()
  {
    return parts_;
  }

  inline
  fid_type
  PartItem::fid_get() const
  {
    return partid_.fid;
  }

  inline
  partnum_type
  PartItem::num_get() const
  {
    return partid_.partnum;
  }

  inline
  unsigned char*
  PartItem::hash_get()
  {
    return &hash_[0];
  }

  inline
  std::vector<stid_type>&
  PartItem::locations_get()
  {
    return locations_;
  }

  inline
  void
  PartItem::add_stid(stid_type id)
  {
    locations_.push_back(id);
  }

  inline
  mtid_type
  MasterItem::id_get() const
  {
    return id_;
  }

  inline
  std::string
  MasterItem::host_addr_get() const
  {
    return host_addr_;
  }

  inline
  stid_type
  StorageItem::id_get() const
  {
    return id_;
  }

  inline
  std::string
  StorageItem::host_addr_get() const
  {
    return host_addr_;
  }

  inline
  port_type
  StorageItem::port_get() const
  {
    return port_;
  }

  inline
  ADDR
  StorageItem::addr_get() const
  {
    return network::get_addr(host_addr_, port_);
  }

  inline
  avspace_type
  StorageItem::available_space_get() const
  {
    return available_space_;
  }

  inline
  uint64_t
  MetaOnFilesItem::count_get() const
  {
    return count_;
  }

  inline
  void
  MetaOnFilesItem::count_set(uint64_t v)
  {
    count_ = v;
  }

  inline
  uint128_t
  MetaOnFilesItem::total_size_get() const
  {
    return total_size_;
  }

  inline
  void
  MetaOnFilesItem::total_size_set(uint128_t v)
  {
    total_size_ = v;
  }

  inline
  std::unordered_map<fid_type, std::string>&
  MetaOnFilesItem::name_by_id_get()
  {
    return name_by_id_;
  }

  inline
  std::string
  MetaOnFilesItem::file_name_by_id(network::masks::fid_type id) const
  {
    return name_by_id_.at(id);
  }

  inline
  stid_type
  MetaOnStoragesItem::count_get() const
  {
    return count_;
  }

  inline
  void
  MetaOnStoragesItem::count_set(stid_type v)
  {
    count_ = v;
  }

  inline
  uint128_t
  MetaOnStoragesItem::available_space_get() const
  {
    return available_space_;
  }

  inline
  void
  MetaOnStoragesItem::available_space_set(uint128_t v)
  {
    available_space_ = v;
  }
}
