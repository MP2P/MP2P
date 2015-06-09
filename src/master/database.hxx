#include "master.hh"

namespace DB
{
  inline
  void
  Database::cmd_put_file(const std::string& key, const std::string& filename)
  {
    std::ifstream file(filename);
    if (!file.good())
      throw std::logic_error("Could not find " + filename + " file.");
    std::string contents((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));
    this->cmd_put(key, contents);
    file.close();
  }

  inline
  CouchbaseDb::CouchbaseDb(const std::string& host,
                           const std::string& pass,
                           const std::string& bucket)
      : Database{},
        client_{"couchbase://" + host + "/" + bucket, pass} // Throws
  {
    Couchbase::Status status = client_.connect();
    if (!status.success())
      throw std::logic_error(
          "Could not connect to 'couchbase://" + host + "/" + bucket + "' ("
          + std::string(status) + ").");
  }

  // Database commands
  inline
  std::string
  CouchbaseDb::cmd_get(const std::string& key)
  {
    auto result = client_.get(key);
    if (!result.status().success())
      throw std::logic_error("Key " + key + " does not exists.");
    return result.value();
  }

  inline
  void
  CouchbaseDb::cmd_remove(const std::string& key)
  {
    client_.remove(key);
  }

  inline
  void
  CouchbaseDb::cmd_put(const std::string& key, const std::string& value)
  {
    auto result = client_.upsert(key, value);
    if (!result.status().success())
      throw std::logic_error("Can't put " + key + ", error: " +
                             std::to_string(result.cas()));
    utils::Logger::cout() << "Upserted " + key;
  }

  // Connector
  inline
  Database&
  Connector::get_instance()
  {
    if (database_.get() == 0)
      throw std::runtime_error("Database is not initialized.");
    return *database_;
  }

  inline
  Database&
  Connector::get_instance(const std::string& host,
                          const std::string& pass,
                          const std::string& bucket)
  {
    if (database_.get() == 0)
    {
      try
      {
        // Throws if anything goes bad
        database_.reset(new CouchbaseDb(host, pass, bucket));
        utils::Logger::cout() << "Successfully connected to database.";
      }
      catch (Couchbase::Status& s)
      {
        throw std::runtime_error("Master exception: Invalid database "
                                 "configuration (couchbase://"
                                 + host + "/"
                                 + bucket + ").");
      }
    }
    try
    {
      database_->cmd_get("storage_sizes");
    }
    catch (std::logic_error)
    {
      database_->cmd_put_file("storage_sizes", "../ressources/data/storage_sizes.json");
      utils::Logger::cerr() << "Added storage_sizes in database.";
    }
    try
    {
      database_->cmd_get("storages");
    }
    catch (std::logic_error)
    {
      database_->cmd_put("storages", "{\"count\":0, \"available_space\":0}");
      utils::Logger::cerr() << "Added storages in database.";
    }
    try
    {
      database_->cmd_get("files");
    }
    catch (std::logic_error)
    {
      database_->cmd_put("files", "{\"count\":0, \"total_size\":0, \"name_by_id\":[]}");
      utils::Logger::cerr() << "Added files in database.";
    }
    return *database_;
  }



  // Item's constructors
  inline
  FileItem::FileItem(fid_type id, const std::string& name,
                     fsize_type file_size,
                     rdcy_type redundancy,
                     rdcy_type current_redundancy,
                     std::string hash, bool uploaded, std::vector<PartItem> parts)
      : id_{id}, name_{name}, file_size_{file_size}, redundancy_{redundancy},
        current_redundancy_{current_redundancy}, uploaded_{uploaded}, parts_{parts}
  {
    for (size_t j = 0; j < sha1_type_size; ++j)
      hash_[j] = hash.c_str()[j];
  }

  inline
  PartItem::PartItem(const PARTID& partid, std::string hash,
                     const std::vector<stid_type>& locations)
      : partid_{partid.fid, partid.partnum}, locations_{locations}
  {
    for (size_t j = 0; j < sha1_type_size; ++j)
      hash_[j] = hash.c_str()[j];
  }

  inline
  MasterItem::MasterItem(mtid_type id, const std::string& host_addr)
      : id_{id}, host_addr_{host_addr} {};

  inline
  StorageItem::StorageItem(stid_type id, const std::string& host_addr,
                           port_type port, avspace_type available_space)
      : id_{id}, host_addr_{host_addr}, port_{port}, available_space_{available_space} {};

  inline
  MetaOnFilesItem::MetaOnFilesItem(const uint64_t count, uint128_t total_size,
                                   const std::unordered_map<fid_type, std::string>& name_by_id)
      : count_{count}, total_size_{total_size}, name_by_id_{name_by_id} {};


  // Item's serializers
  inline std::string
  FileItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << utils::misc::string_from(id_) << ','
          << "\"name\": " << utils::misc::string_from(name_) << ','
          << "\"file_size\":" << utils::misc::string_from(file_size_) << ','
          << "\"redundancy\":" << utils::misc::string_from(redundancy_) << ','
          << "\"current_redundancy\":" << utils::misc::string_from(current_redundancy_) << ','
          << "\"hash\":" << utils::misc::string_from(hash_, sha1_type_size) << ','
          << "\"uploaded\":" << utils::misc::string_from(uploaded_) << ','
          << "\"parts\":[";
          auto it = parts_.cbegin();
          auto end = parts_.cend();
          if (it != end)
            ss << it->serialize();
          ++it;
          for (; it != end; ++it)
            ss << "," << it->serialize();
          ss << "]"
    << "}";
    return ss.str();
  }

  inline std::string
  PartItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"partid_fid\":" << utils::misc::string_from(partid_.fid) << ','
          << "\"partid_partnum\":" << utils::misc::string_from(partid_.partnum) << ','
          << "\"hash\":" << utils::misc::string_from(hash_, sha1_type_size) << ','
          << "\"locations\":[" << utils::misc::separate(locations_, ",") << "]"
    << "}";
    return ss.str();
  }

  inline std::string
  MasterItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << utils::misc::string_from(id_) << ','
          << "\"host_addr\":" << utils::misc::string_from(host_addr_)
    << "}";
    return ss.str();
  }

  inline std::string
  StorageItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << utils::misc::string_from(id_) << ','
          << "\"host_addr\":" << utils::misc::string_from(host_addr_) << ','
          << "\"port\":" << utils::misc::string_from(port_) << ','
          << "\"available_space\":" << utils::misc::string_from(available_space_)
    << "}";
    return ss.str();
  }

  inline std::string
  MetaOnFilesItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"count\":" << utils::misc::string_from(count_) << ','
          << "\"total_size\":" << utils::misc::string_from(total_size_) << ','
          << "\"name_by_id\":[";
          auto it = name_by_id_.cbegin();
          auto end = name_by_id_.cend();
          if (it != end)
            ss << "{\"id\":" << it->first << ",\"name\":"
               << utils::misc::string_from(it->second) << "}";
          ++it;
          for (; it != end; ++it)
            ss << "," << "{\"id\":" << it->first << ",\"name\":"
               << utils::misc::string_from(it->second) << "}";
       ss << "]"
    << "}";
    return ss.str();
  }


  using boost::property_tree::read_json;
  using boost::property_tree::write_json;

  // Item's deserializers
  inline PartItem
  PartItem::deserialize(boost::property_tree::ptree& pt)
  {
    PARTID partid;
    std::string hash;
    std::vector<stid_type> locations;

    partid.fid = pt.get<fid_type>("partid_fid");
    partid.partnum = pt.get<partnum_type>("partid_partnum");
    hash = pt.get<std::string>("hash");

    boost::property_tree::ptree locs_pt = pt.get_child("locations");

    for (auto it = locs_pt.begin(); it != locs_pt.end(); ++it)
    {
      locations.push_back(boost::lexical_cast<stid_type>(it->second.data()));
    }
    return PartItem(partid, hash, locations);
  }

  // Item's deserializers
  inline PartItem
  PartItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::istringstream is(json);
    boost::property_tree::read_json(is, pt);
    return PartItem::deserialize(pt);
  }

  inline FileItem
  FileItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::istringstream is(json);
    boost::property_tree::read_json(is, pt);
    fid_type id = pt.get<fid_type>("id");
    std::string name = pt.get<std::string>("name");
    fsize_type file_size = pt.get<fsize_type>("file_size");
    rdcy_type redundancy = pt.get<rdcy_type>("redundancy");
    rdcy_type current_redundancy = pt.get<rdcy_type>("current_redundancy");
    std::string hash = pt.get<std::string>("hash");
    bool uploaded = pt.get<bool>("uploaded");

    std::vector<PartItem> parts;
    boost::property_tree::ptree locs_pt = pt.get_child("parts");
    for (auto v : locs_pt)
      parts.push_back(PartItem::deserialize(v.second));
    return FileItem(id, name, file_size, redundancy, current_redundancy, hash,
                    uploaded, parts);
  }

  inline MasterItem
  MasterItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::stringstream is(json);
    boost::property_tree::read_json(is, pt);

    mtid_type id = pt.get<mtid_type>("id");
    std::string host_addr = pt.get<std::string>("host_addr");

    return MasterItem(id, host_addr);
  }

  inline StorageItem
  StorageItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::istringstream is(json);
    boost::property_tree::read_json(is, pt);

    stid_type id = pt.get<stid_type>("id");
    std::string host_addr = pt.get<std::string>("host_addr");
    port_type port = pt.get<port_type>("port");
    avspace_type available_space = pt.get<avspace_type>("available_space");

    return StorageItem(id, host_addr, port, available_space);
  }

  inline MetaOnFilesItem
  MetaOnFilesItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::istringstream is(json);
    boost::property_tree::read_json(is, pt);

    uint64_t count = pt.get<uint64_t>("count");
    uint128_t total_size = pt.get<uint128_t>("total_size");
    std::unordered_map<fid_type, std::string> name_by_id;

    boost::property_tree::ptree locs_pt = pt.get_child("name_by_id");
    for (auto v : locs_pt)
      name_by_id.emplace(v.second.get<fid_type>("id"),
                         v.second.get<std::string>("name"));

    return MetaOnFilesItem(count, total_size, name_by_id);
  }
};
