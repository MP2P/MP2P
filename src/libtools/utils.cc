#include <iostream>
#include <libtools.hh>

# include <libconfig.h++>

const bool debug = true;


namespace utils
{
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

  unsigned get_port(std::unique_ptr<libconfig::Config>& config)
  {
    unsigned port = 0;
    /// Getting the port number to be binded
    if (!config->lookupValue("server.master.bind_port", port))
      port = 3727; // #MP2P
    return port;
  }
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
