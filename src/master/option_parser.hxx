#include "boost/program_options.hpp"

namespace master
{
  namespace po = boost::program_options;

  void
  parse_options(int argc, const char *argv[], conf& config)
  {
    try
    {
      // Declare the supported options.
      po::options_description desc("Allowed options");
      desc.add_options()
          ("help,h", "display this help and exit")
          ("version,v", "displays version of the master")
          ("config,c",  po::value<std::string>(), "path to the configuration file")
          ("host,h",  po::value<std::string>(), "the host/ip to listen")
          ("port,p",  po::value<network::masks::port_type>(), "the port to bind")
          ("concurrency,y", po::value<unsigned>(), "the number of thread to listen")
          ("timeout,t", po::value<unsigned>(), "timeout in seconds ")
          ;

      // Parse options.
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);

      if (vm.count("help"))
      {
        std::cout << desc << std::endl;
        throw 0;
      }

      if (vm.count("version"))
      {
        std::cout << "Master " << infos::version << std::endl << std::endl
        << infos::copyright << std::endl;
        throw 0;
      }

      if (vm.count("config"))
        config.config_path = vm["config"].as<std::string>();
      else
        config.config_path = "../config/server.conf";

      if (vm.count("host"))
        config.host = vm["host"].as<std::string>();
      else
        config.host = "localhost";

      if (vm.count("port"))
        config.port = vm["port"].as<network::masks::port_type>();
      else
        config.port = 3727;

      if (vm.count("concurrency"))
        config.concurrency = vm["concurrency"].as<unsigned>();
      else
        config.concurrency = 1;

      if (vm.count("timeout"))
        config.timeout = vm["timeout"].as<unsigned>();
      else
        config.timeout = 1;
    }
    catch (std::exception &e)
    {
      std::cerr << "Invalid option(s). Try --help." << std::endl;
      throw 1;
    }
  }
}