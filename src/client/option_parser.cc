#include "client.hh"

#include <boost/filesystem.hpp>
#include "boost/program_options.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define DEFAULT_CONFIG_PATH "../config/client.conf"
#define DEFAULT_MASTER_HOSTNAME "localhost"
#define DEFAULT_MASTER_PORT 3727
#define DEFAULT_CONCURRENCY 4


using c_ty = decltype(client::conf.config_path);
using mh_ty = decltype(client::conf.master_hostname);
using mp_ty = decltype(client::conf.master_port);
using cy_ty = decltype(client::conf.concurrency);
using fp_ty = decltype(client::conf.file_path);
using ry_ty = decltype(client::conf.redundancy);
using m_ty = std::pair<mh_ty, mp_ty>;

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

namespace client
{
  namespace po = boost::program_options;

  void
  parse_options_file(const std::string& config_path)
  {
    std::ifstream fs(config_path);
    if (!fs)
      throw std::runtime_error("File not found: " + config_path + ".");

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(fs, pt);

    client::conf.concurrency = pt.get<cy_ty>("concurrency");
    client::conf.master_hostname = pt.get<mh_ty>("master.hostname");
    client::conf.master_port = pt.get<mp_ty>("master.port");
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
          ("concurrency,y", po::value<cy_ty>(), "concurrency level when uploading")
          ("master,m", po::value<m_ty>(), "the hostname and port of the master")
          ("upload,u",  po::value<fp_ty>(), "upload the input file")
          ("download,d",  po::value<fp_ty>(), "download the input file")
          ("redundancy,r", po::value<ry_ty>(), "redundancy level when uploading")
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
        std::cout << "Client " << infos::version << std::endl << std::endl
        << infos::copyright << std::endl;
        throw 0;
      }

      if (!vm.count("upload") && !vm.count("download"))
        throw 0;

      // First define default values...
      client::conf.config_path = DEFAULT_CONFIG_PATH;
      client::conf.concurrency = std::thread::hardware_concurrency();
      client::conf.action = action::none;
      client::conf.file_path = "";
      client::conf.redundancy = 0;
      client::conf.master_hostname = DEFAULT_MASTER_HOSTNAME;
      client::conf.master_port = DEFAULT_MASTER_PORT;

      // Second parse options specified in config file...
      if (vm.count("config"))
        client::conf.config_path = vm["config"].as<c_ty>();
      parse_options_file(client::conf.config_path);

      // ... then override them by specified CLI arguments if any...
      if (vm.count("master"))
      {
        auto master = vm["master"].as<m_ty>();
        client::conf.master_hostname = master.first;
        if (master.second)
          client::conf.master_port = master.second;
      }

      if (vm.count("upload"))
      {
        if (!vm.count("redundancy"))
          throw std::runtime_error("Please provide a redundancy with -r.");

        client::conf.action = action::upload;

        std::cout << "Trying to upload file: "
                     + vm["upload"].as<fp_ty>()
                     + " (redundancy=" + utils::misc::string_from(
            vm["redundancy"].as<ry_ty>()) + ")." << std::endl;

        client::conf.file_path = vm["upload"].as<fp_ty>();
        client::conf.redundancy = vm["redundancy"].as<ry_ty>();
      }
      else if (vm.count("download"))
      {
        client::conf.action = action::download;

        std::cout << "Trying to download file: "
                     + vm["download"].as<fp_ty>() + "." << std::endl;

        client::conf.file_path = vm["upload"].as<fp_ty>();
      }

      // ... and finally check results
      if (client::conf.master_hostname == "")
        client::conf.master_hostname = DEFAULT_MASTER_HOSTNAME;
      if (client::conf.master_port == 0)
        client::conf.master_port = DEFAULT_MASTER_PORT;
      if (client::conf.concurrency == 0)
        client::conf.concurrency = DEFAULT_CONCURRENCY;
      if (client::conf.action == action::upload)
        if (client::conf.redundancy == 0)
          throw std::logic_error("Redundancy must be 1 or more.");
      if (client::conf.action != action::none)
        if (!boost::filesystem::exists(client::conf.file_path))
          throw std::logic_error("File does not exists " + client::conf.file_path);

      // Just try to resolve hostnames (throw if it does not work).
      network::get_ipv6(client::conf.master_hostname);
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
  }
}
