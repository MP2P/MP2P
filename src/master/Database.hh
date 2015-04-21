#pragma once

#include <string>
#include <libcouchbase/couchbase++.h>
#include <libcouchbase/couchbase++/views.h>

namespace Database
{

  class Database
  {
  private:
    Database() = default;
    ~Database();

    Database(Database const&) = delete;

    void operator=(Database const&) = delete;

    Couchbase::Client* client_ = NULL;
    Couchbase::Status* status_ = NULL;

  public:
    // Singleton
    static Database& get_instance();

    bool Initialize();

    bool LoadJsonFile(std::string& path);

  };
}

#include "DbConnector.hxx"