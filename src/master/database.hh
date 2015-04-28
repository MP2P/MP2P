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
                const std::string& bucket);
    ~CouchbaseDb() = default;

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
    network::fid_type id_;
    network::fname_type name_;
    network::fsize_type file_size_ = 0;
    network::rdcy_type replication_ = 1;
    network::rdcy_type current_replication_ = 1;
    network::sha1_type hash_;
    bool uploaded_ = false;

  public:
    FileItem() : Item() {};
    network::fsize_type file_size_get() const;
    network::rdcy_type replication_get() const;
    network::rdcy_type current_replication_get() const;
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
  public:
    PartItem() : Item() {};
    network::fid_type fileid_get() const;
    network::partnum_type num_get() const;
    network::sha1_return_type hash_get();
    std::string serialize() const;
  };

  class MasterItem : public Item
  {
  private:
    network::mtid_type id_;
    boost::asio::ip::address_v6 addr_;
  public:
    MasterItem() : Item() {};
    boost::asio::ip::address_v6 const& addr_get() const;
    network::mtid_type id_get() const;
    std::string serialize() const;
  };

  class StorageItem : public Item
  {
  private:
    network::stid_type id_;
    boost::asio::ip::address_v6 addr_;
    network::avspace_type available_space_;
  public:
    StorageItem() : Item() {};
    network::stid_type id_get() const;
    boost::asio::ip::address_v6 const& addr_get() const;
    network::avspace_type available_space_get() const;
    std::string serialize() const;
  };
}

#include "database_items.hxx"
#include "database.hxx"
