#include "storage.hh"

#include <boost/filesystem.hpp>
#include "boost/program_options.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define DEFAULT_CONFIG_PATH "../config/storage.conf"
#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_PORT 3728
#define DEFAULT_MASTER_HOSTNAME "localhost"
#define DEFAULT_MASTER_PORT 3727
#define DEFAULT_CONCURRENCY 4
#define DEFAULT_TIMEOUT 300
#define DEFAULT_STORAGE_PATH "stock_dir"


using c_ty = decltype(storage::Conf::config_path);
using lh_ty = decltype(storage::Conf::hostname);
using lp_ty = decltype(storage::Conf::port);
using l_ty = std::pair<lh_ty, lp_ty>;
using mh_ty = decltype(storage::Conf::master_hostname);
using mp_ty = decltype(storage::Conf::master_port);
using m_ty = std::pair<mh_ty, mp_ty>; // = l_ty
using cy_ty = decltype(storage::Conf::concurrency);
using to_ty = decltype(storage::Conf::timeout);
using sp_ty = decltype(storage::Conf::storage_path);

namespace std
{

  // Specifying >> operator for std::pair<mh_ty, mp_ty> to parse std::pair
  // CLI arg type.
  istream& operator>>(istream& is, m_ty& master)
  {
    string tmp;
    is >> tmp;
    const size_t sep = tmp.find_first_of(':');
    if (sep != string::npos) // If found
    {
      master.first = boost::lexical_cast<mh_ty, std::string>(tmp.substr(0, sep));
      master.second = boost::lexical_cast<mp_ty, std::string>(tmp.substr(sep + 1));
    }
    else
    {
      master.first = boost::lexical_cast<mh_ty, std::string>(tmp);
      master.second = 0;
    }
    return is;
  }
}

namespace storage
{
  namespace po = boost::program_options;

  // Definition of the configuration of the storage
  struct Conf conf;

  void
  parse_options_file(const std::string& config_path)
  {
    std::ifstream fs(config_path);
    if (!fs)
      throw std::runtime_error("File not found: " + config_path + ".");

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(fs, pt);

    storage::conf.concurrency = pt.get<cy_ty>("concurrency");
    storage::conf.hostname = pt.get<lh_ty>("hostname");
    storage::conf.port = pt.get<lp_ty>("port");
    storage::conf.timeout = pt.get<to_ty>("timeout");
    storage::conf.master_hostname = pt.get<mh_ty>("master.hostname");
    storage::conf.master_port = pt.get<mp_ty>("master.port");
    storage::conf.storage_path = pt.get<sp_ty>("storage_path");
  }

  void
  parse_options(int argc, const char *argv[])
  {
    try
    {
      // Declare the supported options.
      po::options_description desc("Allowed options");
      desc.add_options()
          ("help,h", "display this help and exit")
          ("version,v", "displays version of the client")
          ("config,c",  po::value<c_ty>(), "path to the configuration file")
          ("listen,l",  po::value<l_ty>(), "the host:ip to listen")
          ("master,m", po::value<m_ty>(), "the hostname and port of the master")
          ("concurrency,y", po::value<cy_ty>(), "concurrency level when uploading")
          ("timeout,t", po::value<to_ty>(), "timeout in seconds")
          ;

      // Parse options.
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);

      if (vm.count("help"))
      {
        std::cout << desc;
        throw 0;
      }

      if (vm.count("version"))
      {
        std::cout << "Storage " << infos::version << std::endl << std::endl
        << infos::copyright << std::endl;
        throw 0;
      }

      // First define default values...
      storage::conf.config_path = DEFAULT_CONFIG_PATH;
      storage::conf.hostname = DEFAULT_HOSTNAME;
      storage::conf.port = DEFAULT_PORT;
      storage::conf.master_hostname = DEFAULT_MASTER_HOSTNAME;
      storage::conf.master_port = DEFAULT_MASTER_PORT;
      storage::conf.concurrency = std::thread::hardware_concurrency() * 2;
      storage::conf.timeout = DEFAULT_TIMEOUT;
      storage::conf.storage_path = DEFAULT_STORAGE_PATH;

      // Second parse options specified in config file...
      if (vm.count("config"))
        storage::conf.config_path = vm["config"].as<c_ty>();
      parse_options_file(storage::conf.config_path);

      // ... then override them by specified CLI arguments if any...
      if (vm.count("master"))
      {
        auto master = vm["master"].as<m_ty>();
        storage::conf.master_hostname = master.first;
        if (master.second)
          storage::conf.master_port = master.second;
      }

      if (vm.count("listen"))
      {
        auto listen = vm["listen"].as<l_ty>();
        storage::conf.hostname = listen.first;
        if (listen.second)
          storage::conf.port = listen.second;
      }
      if (vm.count("concurrency"))
        storage::conf.concurrency = vm["concurrency"].as<cy_ty>();

      if (vm.count("timeout"))
        storage::conf.timeout = vm["timeout"].as<to_ty>();

      if (vm.count("storage_path"))
        storage::conf.storage_path = vm["storage_path"].as<sp_ty>();

      // ... and finally check results
      if (storage::conf.hostname == "")
        storage::conf.hostname = DEFAULT_HOSTNAME;
      if (storage::conf.port == 0)
        storage::conf.port = DEFAULT_PORT;
      if (storage::conf.master_hostname == "")
        storage::conf.master_hostname = DEFAULT_MASTER_HOSTNAME;
      if (storage::conf.master_port == 0)
        storage::conf.master_port = DEFAULT_MASTER_PORT;
      if (storage::conf.concurrency == 0)
        storage::conf.concurrency = DEFAULT_CONCURRENCY;
      if (storage::conf.timeout == 0)
        storage::conf.timeout = DEFAULT_TIMEOUT;
      if (!boost::filesystem::exists(storage::conf.storage_path))
        boost::filesystem::create_directory(storage::conf.storage_path);

      // Just try to resolve hostnames (throw if it does not work).
      network::get_ipv6(storage::conf.hostname);
      network::get_ipv6(storage::conf.master_hostname);
    }
    catch (int i)
    {
      throw i;
    }
    catch (std::exception &e)
    {
      std::cerr << e.what() << " Try --help." << std::endl;
      throw 1;
    }
    catch (...)
    {
      std::cerr << "Invalid option(s). Try --help." << std::endl;
      throw 1;
    }
    std::cout << storage::conf.hostname << std::endl;
  }
}
