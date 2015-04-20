#include <utils.hh>

inline bool DbConnector::Initialize()
{
  const std::string host = utils::Conf::get_instance().DBhost_get();
//  const unsigned port = utils::Conf::get_instance().DBport_get();
  const std::string password = utils::Conf::get_instance().DBpassword_get();
  const std::string bucket = utils::Conf::get_instance().DBbucket_get();

  const std::string conf("couchbase://" + host + "/" + bucket);

//  client_ = Couchbase::Client(conf, password);
//  status_ = client_.connect();

  if (!status_.success())
  {
    utils::Logger::cerr() << "Couldn't connect to ' " + conf + "' ("
                             + std::string(status_) + ").";
    return false;
  }
  return true;
}

inline DbConnector& DbConnector::get_instance()
{
  static DbConnector instance;
  return instance;
}
