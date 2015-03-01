#include <iostream>
#include <libtools.hh>
#include <sys/socket.h>

#include <libconfig.h++>

const bool debug = true;

static const std::string WHITE = "\033[0m";
static const std::string RED = "\033[91m";
static const std::string GREEN = "\033[92m";
static const std::string YELLOW = "\033[93m";
static const std::string BLUE = "\033[94m";
static const std::string PURPLE = "\033[95m";
static const std::string CYAN = "\033[96m";

namespace utils
{
  std::ostream& w(std::ostream& o/*=std::cout*/)
  {
    return o << WHITE;
  }
  std::ostream& r(std::ostream& o/*=std::cout*/)
  {
    return o << RED;
  }
  std::ostream& b(std::ostream& o/*=std::cout*/)
  {
    return o << BLUE;
  }
  std::ostream& c(std::ostream& o/*=std::cout*/)
  {
    return o << CYAN;
  }
  std::ostream& g(std::ostream& o/*=std::cout*/)
  {
    return o << GREEN;
  }
  std::ostream& y(std::ostream& o/*=std::cout*/)
  {
    return o << YELLOW;
  }
  std::ostream& p(std::ostream& o/*=std::cout*/)
  {
    return o << PURPLE;
  }

  bool is_system_ok()
  {
    int sock = socket(PF_INET6, SOCK_STREAM, 0);

    int val = 0;
    unsigned len = 0;
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

  void print(std::ostream& out, std::mutex& wmutex, const std::string& msg)
  {
    time_t now = time(0);
    struct tm  tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    //strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    wmutex.lock();
    g(out);
    out << "<" << buf << "> ";
    w(out);
    out << msg << std::endl;
    wmutex.unlock();
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
