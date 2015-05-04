#include <utils.hh>
#include <typeinfo>

namespace Database
{
  inline
  CouchbaseDb::CouchbaseDb(const std::string& host,
                                  const std::string& pass,
                                  const std::string& bucket)
      : Database{},
        client_{"couchbase://" + host + "/" + bucket, pass}
  {
    Couchbase::Status status = client_.connect();
    if (!status.success())
      throw std::logic_error(
          "Could not connect to 'couchbase://" + host + "/" + bucket + "' ("
          + std::string(status) + ").");
  }

  // Db commands
  inline std::string
  CouchbaseDb::cmd_get(const std::string& key)
  {
    auto result = client_.get(key);
    if (!result.status().success())
      throw std::logic_error("Key " + key + " does not exists.");
    return result.value();
  }

  inline void
  CouchbaseDb::cmd_put(const std::string& key, const std::string& value)
  {
    auto result = client_.upsert(key, value);
    if (!result.status().success())
      throw std::logic_error("Can't put " + key + ", error: " +
                                   std::to_string(result.cas()));
  }

//  inline
//  FileItem::FileItem(network::fid_type id, const network::fname_type& name,
//                     network::fsize_type file_size,
//                     network::rdcy_type redundancy,
//                     network::rdcy_type current_redundancy,
//                     network::sha1_ptr_type hash, bool uploaded)
//      : id_{id}, name_{name}, file_size_{file_size}, redundancy_{redundancy},
//        current_redundancy_{current_redundancy}, uploaded_{uploaded};
//  {
//    memcpy((void*)hash, hash_, network::sha1_type_size);
//  }

  inline
  PartItem::PartItem(const network::PARTID& partid,
                     network::sha1_ptr_type hash,
                     const std::vector<network::stid_type>& locations)
      : locations_{locations}
  {
    memcpy((void*)&partid, &partid_, sizeof(network::PARTID));
    memcpy((void*)hash, hash_, network::sha1_type_size);
  }

  inline std::string
  FileItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << network::string_from(id_) << ','
          << "\"file_size\":" << network::string_from(file_size_) << ','
          << "\"redundancy\":" << network::string_from(redundancy_) << ','
          << "\"current_redundancy\":" << network::string_from(current_redundancy_) << ','
          << "\"hash\":" << network::string_from(hash_, network::sha1_type_size)
      << "}";
    return ss.str();
  }

//  Item
//  FileItem::deserialize(std::string& json)
//  {
//  }

  inline std::string
  PartItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"partid\": {"
                          << network::string_from(partid_.fid) << ','
                          << network::string_from(partid_.partnum)
                          << "},"
          << "\"hash\":" << network::string_from(hash_, network::sha1_type_size) << ','
          << "\"locations\": [" << utils::misc::separate(locations_, ",") << "]"
    << "}";
    return ss.str();
  }

  inline std::string
  MasterItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << network::string_from(id_) << ','
          << "\"host_addr\":" << network::string_from(host_addr_) << ','
    << "}";
    return ss.str();
  }

  inline std::string
  StorageItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << network::string_from(id_) << ','
          << "\"host_addr\":" << network::string_from(host_addr_) << ','
          << "\"available_space\":" << network::string_from(available_space_)
    << "}";
    return ss.str();
  }
};