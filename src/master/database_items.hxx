#include <masks/blocks.hh>

namespace Database
{
  inline fsize_type FileItem::file_size_get() const
  {
    return file_size_;
  }

  inline rdcy_type FileItem::replication_get() const
  {
    return replication_;
  }

  inline fid_type FileItem::id_get() const
  {
    return id_;
  }

  sha1_return_type FileItem::hash_get()
  {
    return &hash_[0];
  }

  inline bool FileItem::is_replicated() const
  {
    return replicated_;
  }

  inline bool FileItem::is_uploaded() const
  {
    return uploaded_;
  }

  inline fid_type PartItem::fileid_get() const
  {
    return partid_.fid;
  }

  inline partnum_type PartItem::num_get() const
  {
    return partid_.partnum;
  }

  inline sha1_return_type PartItem::hash_get()
  {
    return &hash_[0];
  }

  inline mtid_type MasterItem::id_get() const
  {
    return id_;
  }

  inline stid_type StorageItem::id_get() const
  {
    return id_;
  }

  inline avspace_type StorageItem::available_space_get() const
  {
    return available_space_;
  }
}