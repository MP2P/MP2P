namespace Database
{
  inline CouchbaseDb::CouchbaseDb(const std::string& host, const std::string& pass,
              const std::string& buckt)
      : Database{},
        client_{"couchbase://" + host + "/" + buckt, pass},
        status_{client_.connect()} // Throws
  {
    if (!status_.success())
      throw std::logic_error(
          "Could not connect to 'couchbase://" + host + "/" + buckt + "' ("
          + std::string(status_) + ").");
  }

  // Db commands
  inline std::string CouchbaseDb::cmd_get(const std::string& key)
  {
    //FIXME
    //Couchbase::GetOperation cmd(key);
    //auto l = cmd.run(client_);
    //utils::Logger::cout() << "Got value1: " + std::string(l.value());
    //std::string s = std::string(cmd.response().value());
    //utils::Logger::cout() << "Got value2: " + s;
    //return s;
    return key; //Just so that it could compile.
  }

  inline bool CouchbaseDb::cmd_put(const std::string& key, const std::string& value)
  {
    //FIXME
    //Couchbase::UpsertOperation scmd(key, value);
    //auto sres = scmd.run(client_);
    //uint64_t result_code = sres.cas();
    //if (result_code != 0)
      //utils::Logger::cerr() << "Can't put value, error: " + std::to_string(sres.cas());
    //return result_code == 0;
    std::string a = key;
    std::string b = value;
    return 0; //Just so that it could compile.
  }

  inline std::string FileItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << string_from(id_) << ','
          << "\"file_size\":" << string_from(file_size_) << ','
          << "\"replication\":" << string_from(replication_) << ','
          << "\"current_replication\":" << string_from(current_replication_) << ','
          << "\"hash\":" << string_from(hash_, sha1_type_size)
      << "}";
    return ss.str();
  }

  inline std::string PartItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
    << "}";
    return ss.str();
  }

  inline std::string MasterItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
    << "}";
    return ss.str();
  }

  inline std::string StorageItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
    << "}";
    return ss.str();
  }
}
