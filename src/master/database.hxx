namespace Database
{
  inline CouchbaseDb::CouchbaseDb(const std::string& host,
                                  const std::string& pass,
                                  const std::string& bucket)
      : Database{},
        client_{"couchbase://" + host + "/" + bucket, pass}
  {
    Couchbase::Status status = client_.connect();
    if (!status.success())
      throw std::logic_error(
          "Could not connect to 'couchbase://" + host + "/" + bucket + "' ("
          + std::string(status) + ").");
  }

  // Db commands
  inline std::string CouchbaseDb::cmd_get(const std::string& key)
  {
    auto result = client_.get(key);
    if (!result.status().success())
      throw std::logic_error("Key " + key + " does not exists.");
    return result.value();
  }

  inline void CouchbaseDb::cmd_put(const std::string& key, const std::string& value)
  {
    auto result = client_.upsert(key, value);
    if (!result.status().success())
      throw std::logic_error("Can't put " + key + ", error: " +
                                   std::to_string(result.cas()));
  }

  inline std::string FileItem::serialize() const
  {
    std::stringstream ss;
    ss << "{"
          << "\"id\":" << network::string_from(id_) << ','
          << "\"file_size\":" << network::string_from(file_size_) << ','
          << "\"redundancy\":" << network::string_from(redundancy_) << ','
          << "\"current_redundancy\":" << network::string_from(current_redundancy_) << ','
          << "\"hash\":" << network::string_from(hash_, network::sha1_type_size)
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
