#include <utils.hh>
#include "client.hh"
#include "boost/program_options.hpp"

namespace po = boost::program_options;


int main(int argc, const char *argv[])
{
  using namespace client; // Use this namespace inside the function only
  try
  {
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "display this help and exit")
        ("version,v", "displays version of the client")
        ("log,l", "activates verbose")
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
      return 0;
    }

    if (vm.count("version"))
    {
      std::cout << "Client " << infos::version << std::endl << std::endl
                << infos::copyright << std::endl;
      return 0;
    }

    if (!vm.count("upload") && !vm.count("download"))
      return 0;

    bool upload = false; // true = upload - false = download

    if (vm.count("upload"))
    {
      if (!vm.count("redundancy"))
      {
        std::cerr << "Please provide a redundancy with --redundancy or -r."
                 << std::endl;
        return 1;
      }
      std::cout << "Trying to upload file: "
      << vm["upload"].as<std::string>() << " (redundancy="
      << vm["redundancy"].as<int>() << ")." << std::endl;

      upload = true;
    }
    else if (vm.count("download"))
    {
      std::cout << "Trying to download file: "
      << vm["download"].as<std::string>() << "." << std::endl;
    }

    utils::init();

    // Prepare socket
    std::ostringstream port;
    port << utils::Conf::get_instance().port_get();
    const auto& host = utils::Conf::get_instance().host_get();
    utils::Logger::cout() << "Endpoint host = " + host;
    utils::Logger::cout() << "Endpoint port = " + port.str();

    Client client{host, port.str()};

    client.run();

    if (upload)
    {
      // Prepare file
      files::File file(vm["upload"].as<std::string>());

      client.send_file(file, vm["redundancy"].as<int>());
    }

    client.stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Client failed : " << e.what() << std::endl;
  }
}
