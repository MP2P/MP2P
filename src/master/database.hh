#pragma once

#include <string>
#include <libcouchbase/couchbase++.h>

namespace Database
{
  // Abstract class
  class Database
  {
  protected:
    Database() = default;
    Database(Database const&) = delete;
    void operator=(Database const&) = delete;
    ~Database() {};

  public:
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

    // Db commands
    std::string cmd_get(const std::string& key);
    bool cmd_put(const std::string& key, const std::string& value);
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
    size_t nb_part_ = 0;
    size_t file_size_ = 0;
    uint replication_ = 1;
    std::string uid_;
    std::string hash_;
    bool replicated_ = false;
    bool uploaded_ = false;

  public:
    FileItem() : Item() {};
    size_t nb_part_get() const;
    size_t file_size_get() const;
    size_t replication_get() const;
    std::string uid_get() const;
    std::string hash_get() const;
    bool is_replicated() const;
    bool is_uploaded() const;
  };

  class PartItem : public Item
  {
  private:
    size_t size_ = 0;
    std::string uid_;
    std::string hash_;
  public:
    PartItem() : Item() {};
    size_t size_get() const;
    std::string uid_get() const;
    std::string hash_get() const;
  };

  class MasterItem : public Item
  {
  private:
    std::string uid_;
  public:
    MasterItem() : Item() {};
    std::string uid_get() const;
  };

  class StorageItem : public Item
  {
  private:
    std::string uid_;
    size_t available_space_;
  public:
    StorageItem() : Item() {};
    std::string uid_get() const;
    size_t available_space_get() const;
  };
}

#include "database_items.hxx"
#include "database.hxx"