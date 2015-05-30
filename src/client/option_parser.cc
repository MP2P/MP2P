#include "client.hh"

#include "boost/program_options.hpp"

namespace client
{
  namespace po = boost::program_options;

  void
  parse_options(int argc, const char *argv[], client::conf& config)
  {
    try
    {
      // Declare the supported options.
      po::options_description desc("Allowed options");
      desc.add_options()
          ("help,h", "display this help and exit")
          ("version,v", "displays version of the client")
          ("config,c",  po::value<std::string>(), "path to the configuration file")
          ("upload,u",  po::value<std::string>(), "upload the input file")
          ("download,d",  po::value<std::string>(), "download the input file")
          ("redundancy,r", po::value<int>(), "redundancy level when uploading")
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
        std::cout << "Client " << infos::version << std::endl << std::endl
        << infos::copyright << std::endl;
        throw 0;
      }

      if (vm.count("config"))
        config.config_path = vm["config"].as<std::string>();
      else
        config.config_path = "../config/server.conf";

      if (!vm.count("upload") && !vm.count("download"))
        throw 0;

      if (vm.count("upload"))
      {
        if (!vm.count("redundancy"))
        {
          std::cerr << "Please provide a redundancy with --redundancy or -r."
          << std::endl;
          throw 1;
        }
        utils::Logger::cout() << "Trying to upload file: "
                                 + vm["upload"].as<std::string>()
                                 + " (redundancy=" + utils::misc::string_from(
            vm["redundancy"].as<int>()) + ").";

        config.action = action::upload;
        config.file_path = vm["upload"].as<std::string>();
        config.redundancy = vm["redundancy"].as<network::masks::rdcy_type>();
      }
      else if (vm.count("download"))
      {
        utils::Logger::cout() << "Trying to download file: "
                                 + vm["download"].as<std::string>() + ".";

        config.action = action::download;
        config.file_path = vm["upload"].as<std::string>();
        config.redundancy = vm["redundancy"].as<network::masks::rdcy_type>();
      }
    }
    catch (std::exception &e)
    {
      std::cerr << "Invalid option(s). Try --help." << std::endl;
      throw 1;
    }
  }
}