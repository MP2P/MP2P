#include <utils.hh>
#include <typeinfo>
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


namespace Database
{
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



  // Item's constructors
  inline
  FileItem::FileItem(fid_type id, const fname_type& name,
                     fsize_type file_size,
                     rdcy_type redundancy,
                     rdcy_type current_redundancy,
                     std::string hash, bool uploaded)
      : id_{id}, name_{name}, file_size_{file_size}, redundancy_{redundancy},
        current_redundancy_{current_redundancy}, uploaded_{uploaded}
  {
    for (size_t j = 0; j < sha1_type_size; ++j)
      hash_[j] = hash.c_str()[j];
  }

  inline
  PartItem::PartItem(const PARTID& partid, std::string& hash,
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
                           avspace_type available_space)
      : id_{id}, host_addr_{host_addr}, available_space_{available_space} {};



  // Item's serializers
  inline std::string
  FileItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << utils::misc::string_from(id_) << ','
          << "\"name\":" << utils::misc::string_from(name_) << ','
          << "\"file_size\":" << utils::misc::string_from(file_size_) << ','
          << "\"redundancy\":" << utils::misc::string_from(redundancy_) << ','
          << "\"current_redundancy\":" << utils::misc::string_from(current_redundancy_) << ','
          << "\"hash\":" << utils::misc::string_from(hash_, sha1_type_size) << ','
          << "\"uploaded\":" << utils::misc::string_from(uploaded_)
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
          << "\"available_space\":" << utils::misc::string_from(available_space_)
    << "}";
    return ss.str();
  }


  using boost::property_tree::read_json;
  using boost::property_tree::write_json;

  // Item's deserializers
  FileItem
  FileItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::istringstream is(json);
    boost::property_tree::read_json(is, pt);
    fid_type id = pt.get<fid_type>("id");
    fname_type name = pt.get<fname_type>("name");
    fsize_type file_size = pt.get<fsize_type>("file_size");
    rdcy_type redundancy = pt.get<rdcy_type>("redundancy");
    rdcy_type current_redundancy = pt.get<rdcy_type>("current_redundancy");
    std::string hash = pt.get<std::string>("hash");
    bool uploaded = pt.get<bool>("uploaded");

    return FileItem(id, name, file_size, redundancy, current_redundancy, hash,
                    uploaded);
  }

  PartItem
  PartItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::istringstream is(json);
    boost::property_tree::read_json(is, pt);

    PARTID partid;
    std::string hash;
    std::vector<stid_type> locations;

    partid.fid = pt.get<fid_type>("partid_fid");
    partid.partnum = pt.get<partnum_type>("partid_partnum");
    hash = pt.get<std::string>("hash");

    BOOST_FOREACH(const boost::property_tree::ptree::value_type &v,
                  pt.get_child("locations"))
    {
      locations.push_back(std::stoi(v.second.data()));
    }

    return PartItem(partid, hash, locations);
  }

  MasterItem
  MasterItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::stringstream is(json);
    boost::property_tree::read_json(is, pt);

    mtid_type id = pt.get<mtid_type>("id");
    std::string host_addr = pt.get<std::string>("host_addr");

    return MasterItem(id, host_addr);
  }

  StorageItem
  StorageItem::deserialize(std::string& json)
  {
    boost::property_tree::ptree pt;
    std::istringstream is(json);
    boost::property_tree::read_json(is, pt);

    stid_type id = pt.get<stid_type>("id");
    std::string host_addr = pt.get<std::string>("host_addr");
    avspace_type available_space =
        pt.get<avspace_type>("available_space");

    return StorageItem(id, host_addr, available_space);
  }
};