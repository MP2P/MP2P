#include "client.hh"

#include "boost/program_options.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using c_ty = decltype(client::conf.config_path);
using mh_ty = decltype(client::conf.master_hostname);
using mp_ty = decltype(client::conf.master_port);
using cy_ty = decltype(client::conf.concurrency);
using fp_ty = decltype(client::conf.file_path);
using ry_ty = decltype(client::conf.redundancy);
using m_ty = std::pair<mh_ty, mp_ty>;

namespace std {

  // Specifying >> operator for std::pair<mh_ty, mp_ty>to parse std::pair
  // CLI arg type.
  istream& operator>>(istream& is, m_ty& master)
  {
    string tmp;
    is >> tmp;
    const size_t sep = tmp.find(':');
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
    client::conf.master_hostname = pt.get<mh_ty>("master_hostname");
    client::conf.master_port = pt.get<mp_ty>("master_port");
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
          ("concurrency,o", po::value<cy_ty>(), "concurrency level when uploading")
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

      // First parse options specified in config file...
      if (vm.count("config"))
        client::conf.config_path = vm["config"].as<c_ty>();
      else
        client::conf.config_path = "../config/client.conf";
      parse_options_file(client::conf.config_path);

      // ... then override them by specified CLI arguments if any.
      if (vm.count("master"))
      {
        auto master = vm["master"].as<m_ty>();
        client::conf.master_hostname = master.first;
        if (master.second)
          client::conf.master_port = master.second;
      }

      if (!vm.count("upload") && !vm.count("download"))
        throw 0;

      if (vm.count("upload"))
      {
        client::conf.action = action::upload;

        if (!vm.count("redundancy"))
          throw std::runtime_error("Please provide a redundancy with -r.");
        utils::Logger::cout() << "Trying to upload file: "
                                 + vm["upload"].as<fp_ty>()
                                 + " (redundancy=" + utils::misc::string_from(
            vm["redundancy"].as<ry_ty>()) + ").";

        client::conf.file_path = vm["upload"].as<fp_ty>();
        client::conf.redundancy = vm["redundancy"].as<ry_ty>();
      }
      else if (vm.count("download"))
      {
        client::conf.action = action::download;

        utils::Logger::cout() << "Trying to download file: "
                                 + vm["download"].as<fp_ty>() + ".";

        client::conf.file_path = vm["upload"].as<fp_ty>();
        client::conf.redundancy = vm["redundancy"].as<ry_ty>();
      }
    }
    catch (std::exception &e)
    {
      std::cerr << e.what() << " Try --help." << std::endl;
      throw 1;
    }
    catch (int)
    {
      throw 1;
    }
    catch (...)
    {
      std::cerr << "Invalid option(s). Try --help." << std::endl;
      throw 1;
    }
  }
}