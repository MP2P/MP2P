#pragma once

#include <string>
#include <libcouchbase/couchbase++.h>
#include <libcouchbase/couchbase++/views.h>

namespace Database
{

  class Database
  {
  private:
    Couchbase::Client* client_ = NULL;
    Couchbase::Status* status_ = NULL;

    Database() = default;
    Database(Database const&) = delete;
    void operator=(Database const&) = delete;
    ~Database();

  public:
    // Singleton
    static Database& get_instance();

    bool Initialize();

    bool load_json_file(std::string& path);
  };

  class FileItem
  {
  private:
    size_t nb_part_ = 0;
    size_t file_size_ = 0;
    uint replication_ = 0;
    bool replicated_ = false;
    bool uploaded_ = false;

  public:
    size_t nb_part_get() const;
    size_t file_size_get() const;
    size_t replication_get() const;
    bool is_replicated() const;
    bool is_uploaded() const;
  };

  class PartItem
  {
  private:
    size_t size_ = 0;
    std::string uid_;
    std::string hash_;
  public:
    size_t size_get() const;
    std::string uid_get() const;
    std::string hash_get() const;
  };

}

#include "DbConnector.hxx"
#include "DbItems.hxx"