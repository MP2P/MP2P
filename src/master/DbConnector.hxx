#include <utils.hh>

namespace Database {

  inline Database& Database::get_instance()
  {
    static Database instance;
    return instance;
  }

  inline Database::~Database()
  {
    if (client_ != NULL)
      delete client_;
    if (status_ != NULL)
      delete status_;
  }

  inline bool Database::Initialize()
  {
    const std::string host = utils::Conf::get_instance().DBhost_get();
    const std::string password = utils::Conf::get_instance().DBpassword_get();
    const std::string bucket = utils::Conf::get_instance().DBbucket_get();

    const std::string conf("couchbase://" + host + "/" + bucket);

    client_ = new Couchbase::Client(conf, password);
    status_ = new Couchbase::Status(client_->connect());

    if (!status_->success())
    {
      utils::Logger::cerr() << "Couldn't connect to ' " + conf + "' ("
                               + std::string(*status_) + ").";
      return false;
    }

    return true;
  }

  inline bool Database::load_json_file(std::string& path)
  {
    // TODO
    if (path != "")
      return false;
    return false;
  }
}