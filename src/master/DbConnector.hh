#pragma once

#include <string>
#include <libcouchbase/couchbase++.h>
//#include <libcouchbase/couchbase++/views.h>

class DbConnector
{
private:
  DbConnector() = default;
  DbConnector(DbConnector const& ) = delete;
  void operator=(DbConnector const& ) = delete;

  Couchbase::Client client_;
  Couchbase::Status status_;

public:
  // Singleton
  static DbConnector& get_instance();

  bool Initialize();
};

#include "DbConnector.hxx"