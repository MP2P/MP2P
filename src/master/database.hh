#pragma once

#include <string>
#include <libcouchbase/couchbase++.h>
#include <boost/asio/ip/address_v6.hpp>

#include <masks/blocks.hh>

namespace Database
{
  // Abstract class
  class Database
  {
  protected:
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
                const std::s&tring& buckt);
    ~CouchbaseDb() { Database::~Database(); };

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
    virtual std::string serialize() const = 0;
  };

  class FileItem : public Item
  {
  private:
    fid_type id_;
    fname_type name_;
    fsize_type file_size_ = 0;
    rdcy_type replication_ = 1;
    rdcy_type current_replication_ = 1;
    sha1_type hash_;
    bool uploaded_ = false;

  public:
    FileItem() : Item() {};
    fsize_type file_size_get() const;
    rdcy_type replication_get() const;
    rdcy_type current_replication_get() const;
    fid_type id_get() const;
    sha1_return_type hash_get();
    bool is_replicated() const;
    bool is_uploaded() const;
    std::string serialize() const;
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
    std::string serialize() const;
  };

  class MasterItem : public Item
  {
  private:
    mtid_type id_;
    boost::asio::ip::address_v6 addr_;
  public:
    MasterItem() : Item() {};
    boost::asio::ip::address_v6 const& addr_get() const;
    mtid_type id_get() const;
    std::string serialize() const;
  };

  class StorageItem : public Item
  {
  private:
    stid_type id_;
    boost::asio::ip::address_v6 addr_;
    avspace_type available_space_;
  public:
    StorageItem() : Item() {};
    stid_type id_get() const;
    boost::asio::ip::address_v6 const& addr_get() const;
    avspace_type available_space_get() const;
    std::string serialize() const;
  };
}

#include "database_items.hxx"
#include "database.hxx"