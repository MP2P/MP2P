#include <iostream>
#include <libtools.hh>
#include <sys/socket.h>

#include <libconfig.h++>

const bool debug = true;


namespace utils
{
  bool is_system_ok()
  {
    int sock = socket(PF_INET6, SOCK_STREAM, 0);

    int val;
    unsigned len;
    if (::getsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &val, &len))
    {
      std::cerr << "Fatal error: IPV6_V6ONLY must be false." << std::endl;
      return false;
    }
    return true;
  }

  void print_debug(const std::string& info)
  {
    if (debug)
      std::cout << info << std::endl;
  }

  std::unique_ptr<libconfig::Config> get_config(const std::string& path)
  {
    std::unique_ptr<libconfig::Config> cfg =
      std::make_unique<libconfig::Config>();

    try
    {
      cfg->readFile(path.c_str());
    }
    catch(const libconfig::FileIOException &fioex)
    {
      std::cerr << "I/O error while reading file." << std::endl;
      return NULL;
    }
    catch(const libconfig::ParseException &pex)
    {
      std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
        << " - " << pex.getError() << std::endl;
      return NULL;
    }

    return cfg;
  }

  /// Getting the port number to be binded
  unsigned get_port(std::unique_ptr<libconfig::Config>& config)
  {
    unsigned port = 0;
    config->lookupValue("server.master.bind_port", port);
    if (port == 0)
      port = 3727; // #MP2P
    return port;
  }

  /// Getting the concurency level
  unsigned get_concurent_threads(std::unique_ptr<libconfig::Config>& config)
  {
    unsigned concurent_threads = 0;
    config->lookupValue("server.master.concurency", concurent_threads);
    if (!concurent_threads)
      concurent_threads = std::thread::hardware_concurrency();
    if (!concurent_threads)
      concurent_threads = 1;
    return concurent_threads;
  }
}
