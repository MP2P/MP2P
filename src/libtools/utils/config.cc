#include <utils.hh>

#include <thread>

namespace utils
{
  Conf::Conf()
  {}

  bool Conf::update_conf(const std::string &path)
  {
    libconfig::Config cfg;

    try
    {
      cfg.readFile(path.c_str());
    }
    catch (const libconfig::FileIOException &fioex)
    {
      std::cerr << "i/o error while reading file." << std::endl;
      return false;
    }
    catch (const libconfig::ParseException &pex)
    {
      std::cerr << "parse error at " << pex.getFile() << ":" << pex.getLine()
          << " - " << pex.getError() << std::endl;
      return false;
    }


    // Getting host value - Default = "localhost"
    cfg.lookupValue("server.host", host_);
    if (host_ == "")
      host_ = "localhost";


    // Getting port value - Default = 3727
    cfg.lookupValue("server.port", port_);
    if (port_ == 0)
      port_ = 3727; // 3727=mp2p


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

    return true;
  }

  /// getting the host to bind
  std::string Conf::get_host()
  {
    return host_;
  }
  /// getting the port number to bind
  unsigned Conf::get_port()
  {
    return port_;
  }

  /// getting the concurrency level
  unsigned Conf::get_concurrency()
  {
    return concurrent_threads_;
  }

  /// getting the timeout for server connexions
  time_duration Conf::get_timeout()
  {
    return timeout_;
  }
}
