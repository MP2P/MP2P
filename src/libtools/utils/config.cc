#include <utils.hh>

#include <thread>

using namespace boost::posix_time;

namespace utils
{
  Conf& Conf::get_instance()
  {
    static Conf instance;
    return instance;
  }

  bool Conf::initialize(const std::string& path)
  {
    libconfig::Config cfg;

    try
    {
      cfg.readFile(path.c_str());
    }
    catch (const libconfig::FileIOException& fioex)
    {
      utils::Logger::cout() << "I/O error while reading configuration file.";
      return false;
    }
    catch (const libconfig::ParseException& pex)
    {
      utils::Logger::cout() << "Parse error while reading configuration file ("
                               + std::string(pex.getFile()) +  ":"
                               + std::to_string(pex.getLine()) + ").";
      return false;
    }


    // Getting host value - Default = "localhost"
    cfg.lookupValue("server.host", host_);
    if (host_ == "")
      host_ = "localhost";

    // Getting port value - Default = 3727

    unsigned tmp_port;
    cfg.lookupValue("server.port", tmp_port);
    if (tmp_port == 0)
      tmp_port = 3727; // 3727=mp2p

    port_ = (unsigned short)tmp_port;

    // Getting concurency level - Default = 1
    cfg.lookupValue("server.concurency", concurrent_threads_);
    if (!concurrent_threads_)
      concurrent_threads_ = std::thread::hardware_concurrency();
    if (!concurrent_threads_)
      concurrent_threads_ = 1;


    // Getting timeout in seconds - Default = 300s
    int timeout = 0;
    cfg.lookupValue("server.timeout", timeout);
    if (timeout <= 0)
      timeout = 300;
    timeout_ = seconds(timeout);

    // Getting host value - Default = "localhost"
    cfg.lookupValue("couchbase.host", DBhost_);
    if (DBhost_ == "")
      DBhost_ = "localhost";

    // Getting port value - Default = 11211
    cfg.lookupValue("couchbase.port", DBport_);
    if (DBport_ == 0)
      DBport_ = 11211;

    // Getting password value
    cfg.lookupValue("couchbase.password", DBpassword_);
    if (DBpassword_ == "")
      DBpassword_ = "";

    // Getting bucket name
    cfg.lookupValue("couchbase.bucket", DBbucket_);
    if (DBbucket_ == "")
      DBbucket_ = "";

    return true;
  }
}
