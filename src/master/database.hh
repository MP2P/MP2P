#pragma once

#include <utils.hh>
#include <masks/blocks.hh>

using namespace network::masks;

#include <map>
#include <string>
#include <sstream>
#include <string.h>
#include <typeinfo>
#include <boost/foreach.hpp>
#include "libcouchbase/couchbase++.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

namespace DB
{
  // Abstract class
  class Database
  {
  protected: // FIXME
  public:
    Database() = default;

    Database(Database const&) = delete;

    void operator=(Database const&) = delete;

  public:
    virtual ~Database() = default;

    // Db commands -> Throws when it fails
    virtual std::string cmd_get(const std::string& key) = 0;
    virtual void cmd_remove(const std::string& key) = 0;
    virtual void cmd_put(const std::string& key, const std::string& value) = 0;
    void cmd_put_file(const std::string& key, const std::string& filename);
  };

  // Singleton
  class Connector
  {
  private:
    Connector() = default;
    Connector(Connector const& ) = delete;
    void operator=(Connector const& ) = delete;
  public:
    static std::unique_ptr<Database> database_;
    // Initializer
    static Database& get_instance(const std::string& host,
                                  const std::string& pass,
                                  const std::string& bucket);
    // Getter
    static Database& get_instance();
  };
  std::unique_ptr<Database> Connector::database_;


  class CouchbaseDb : public Database
  {
  private:
    Couchbase::Client client_;

  public:
    CouchbaseDb(const std::string& host, const std::string& pass,
                const std::string& bucket);

    ~CouchbaseDb() = default;

    // Db commands -> Throws when it fails
    std::string cmd_get(const std::string& key) override;
    void cmd_remove(const std::string& key) override;
    void cmd_put(const std::string& key, const std::string& value) override;
  };

  class Item
  {
  protected:
    Item() = default;

    ~Item() { };
  public:
    virtual std::string serialize() const = 0;
  };

  class PartItem : public Item
  {
  private:
    PARTID partid_;
    sha1_type hash_;
    std::vector<stid_type> locations_;
  public:
    PartItem(const PARTID& partid, std::string hash,
             const std::vector<stid_type>& locations);


    fid_type fid_get() const;
    partnum_type num_get() const;
    unsigned char* hash_get();
    std::vector<stid_type>& locations_get();
    void add_stid(stid_type id);

    std::string serialize() const override;
    static PartItem deserialize(std::string& json);
    static PartItem deserialize(boost::property_tree::ptree& pt);
  };

  class FileItem : public Item
  {
  private:
    fid_type id_;
    std::string name_;
    fsize_type file_size_ = 0;
    rdcy_type redundancy_ = 0;
    rdcy_type current_redundancy_ = 0;
    sha1_type hash_;
    bool uploaded_ = false;
    std::vector<PartItem> parts_;

  public:
    FileItem(fid_type id, const std::string& name,
             fsize_type file_size, rdcy_type redundancy,
             rdcy_type current_redundancy, std::string hash,
             bool uploaded, std::vector<PartItem> parts);

    fid_type id_get() const;
    std::string name_get() const;
    fsize_type file_size_get() const; // FIXME: change name for size_get only.
    rdcy_type redundancy_get() const;
    rdcy_type current_redundancy_get() const;
    unsigned char* hash_get();
    bool is_replicated() const;
    bool is_uploaded() const;
    std::vector<PartItem>& parts_get();

    std::string serialize() const override;
    static FileItem deserialize(std::string& json);
  };

  class MasterItem : public Item
  {
  private:
    mtid_type id_;
    std::string host_addr_;
  public:
    MasterItem(mtid_type id, const std::string& host_addr);

    MasterItem()
        : Item() { };

    mtid_type id_get() const;
    std::string host_addr_get() const;

    std::string serialize() const override;
    static MasterItem deserialize(std::string& json);
  };

  class StorageItem : public Item
  {
  public:
    StorageItem(stid_type id, const std::string& host_addr,
                port_type port, avspace_type available_space);

  private:
    stid_type id_;
    std::string host_addr_;
    port_type port_;
    avspace_type available_space_;
  public:
    StorageItem()
        : Item() { };

    stid_type id_get() const;
    std::string host_addr_get() const;
    port_type port_get() const;
    ADDR addr_get() const;
    avspace_type available_space_get() const;

    std::string serialize() const override;
    static StorageItem deserialize(std::string& json);
  };

  class MetaOnFilesItem : public Item
  {
  private:
    uint64_t count_;
    uint128_t total_size_;
    std::unordered_map<fid_type, std::string> name_by_id_;
  public:
    MetaOnFilesItem(const uint64_t count, uint128_t total_size,
                    const std::unordered_map<fid_type, std::string>& name_by_id);

    uint64_t count_get() const;
    void count_set(uint64_t v);
    uint128_t total_size_get() const;
    void total_size_set(uint128_t v);
    std::unordered_map<fid_type, std::string>& name_by_id_get();

    std::string file_name_by_id(fid_type id) const;

    std::string serialize() const override;
    static MetaOnFilesItem deserialize(std::string& json);
  };

  namespace tools
  {
    // FIXME: create a smart templated function that try to get a value from DB
    //        with access to value like "my.key.json_key.value.something"
    //        where "my.key" is the DB key, "json_key" is a key in the json
    //        retrieved from "my.key" etc...
    //        The type of the value returned is given by the templated type of
    //        this function.

    network::masks::partnum_type number_of_parts(fsize_type file_size);
    std::vector<StorageItem> get_all_storages();
    ADDR get_storage_addr(stid_type id);

    FileItem create_new_file(std::string name, fsize_type file_size,
                             rdcy_type redundancy, std::string hash);
    std::vector<STPFIELD> get_stpfields_for_upload(FileItem fi);
  }
}

#include "database.hxx"
#include "database_items.hxx"
#include "database_procedures.hxx"
