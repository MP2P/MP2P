#pragma once

#include <string>
#include <libcouchbase/couchbase++.h>

#include <masks/blocks.hh>

namespace Database
{
  // Abstract class
  class Database
  {
  protected:
  public:
    Database() = default;
    Database(Database const&) = delete;
    void operator=(Database const&) = delete;

  public:
    virtual ~Database() = default;

    // Db commands -> Throws when it fails
    virtual std::string cmd_get(const std::string& key) = 0;
    virtual void cmd_put(const std::string& key, const std::string& value) = 0;
  };

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
    void cmd_put(const std::string& key, const std::string& value) override;
  };

  class Item
  {
  protected:
    Item() = default;
    ~Item() {};
  public:
    virtual std::string serialize() const = 0;
  };

  class FileItem : public Item
  {
  private:
    network::fid_type id_;
    network::fname_type name_;
    network::fsize_type file_size_ = 0;
    network::rdcy_type redundancy_ = 1;
    network::rdcy_type current_redundancy_ = 1;
    network::sha1_type hash_;
    bool uploaded_ = false;

  public:
    FileItem() : Item() {};
    network::fsize_type file_size_get() const;
    network::rdcy_type redundancy_get() const;
    network::rdcy_type current_redundancy_get() const;
    network::fid_type id_get() const;
    network::sha1_return_type hash_get();
    bool is_replicated() const;
    bool is_uploaded() const;
    std::string serialize() const;
  };

  class PartItem : public Item
  {
  private:
    network::PARTID partid_;
    network::sha1_type hash_;
    std::vector<network::stid_type> locations_;
  public:
    PartItem() : Item() {};
    network::fid_type fileid_get() const;
    network::partnum_type num_get() const;
    network::sha1_return_type hash_get();
    std::string serialize() const;
    std::vector<network::stid_type> locations_get() const;
  };

  class MasterItem : public Item
  {
  private:
    network::mtid_type id_;
    std::string host_addr_;
  public:
    MasterItem() : Item() {};
    std::string host_addr_get() const;
    network::mtid_type id_get() const;
    std::string serialize() const;
  };

  class StorageItem : public Item
  {
  private:
    network::stid_type id_;
    std::string host_addr_;
    network::avspace_type available_space_;
  public:
    StorageItem() : Item() {};
    network::stid_type id_get() const;
    std::string host_addr_get() const;
    network::avspace_type available_space_get() const;
    std::string serialize() const;
  };
}

#include "database.hxx"
#include "database_items.hxx"
