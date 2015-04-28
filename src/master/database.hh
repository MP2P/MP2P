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

    // Db commands
    virtual std::string cmd_get(const std::string& key) = 0;
    virtual bool cmd_put(const std::string& key, const std::string& value) = 0;
  };

  class CouchbaseDb : public Database
  {
  private:
    Couchbase::Client client_;
    Couchbase::Status status_;

  public:
    CouchbaseDb(const std::string& host, const std::string& pass,
                const std::string& buckt);
    ~CouchbaseDb() {/* Database::~Database(); */};

    // Db commands
    std::string cmd_get(const std::string& key) override;
    bool cmd_put(const std::string& key, const std::string& value) override;
  };

  class Item
  {
  protected:
    Item() = default;
    ~Item() {};
  public:
    virtual std::string serialize() = 0;
  };

  class FileItem : public Item
  {
  private:
    fsize_type file_size_ = 0;
    rdcy_type replication_ = 1;
    fid_type id_;
    sha1_type hash_;
    bool replicated_ = false;
    bool uploaded_ = false;

  public:
    FileItem() : Item() {};
    fsize_type file_size_get() const;
    rdcy_type replication_get() const;
    fid_type id_get() const;
    sha1_return_type hash_get();
    bool is_replicated() const;
    bool is_uploaded() const;
  };

  class PartItem : public Item
  {
  private:
    PARTID partid_;
    sha1_type hash_;
  public:
    PartItem() : Item() {};
    fid_type fileid_get() const;
    partnum_type num_get() const;
    sha1_return_type hash_get();
  };

  class MasterItem : public Item
  {
  private:
    mtid_type id_;
  public:
    MasterItem() : Item() {};
    mtid_type id_get() const;
  };

  class StorageItem : public Item
  {
  private:
    stid_type id_;
    avspace_type available_space_;
  public:
    StorageItem() : Item() {};
    stid_type id_get() const;
    avspace_type available_space_get() const;
  };
}

#include "database_items.hxx"
#include "database.hxx"
