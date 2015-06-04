#include <utils.hh>
#include "client.hh"


int main(int argc, const char *argv[])
{
  using namespace client; // Use this namespace inside the function only

  try
  {
    parse_options(argc, argv);
    utils::check_system();
  }
  catch (int i)
  {
    return i;
  }
  catch (std::exception& e)
  {
    std::cerr << "Initialization failed " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Fatal error: could not initialize." << std::endl;
    return 1;
  }

  try
  {
    Client client{client::conf.master_hostname, client::conf.master_port};

    client.run();

    if (client::conf.action == action::upload)
    {
      // Prepare file
      files::File file(client::conf.file_path);
      client.send_file(file, client::conf.redundancy);
    }

    client.stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Client failed: " + std::string(e.what());
    return 1;
  }
  catch (...)
  {
    utils::Logger::cerr() << "Client: Fatal error.";
    return 1;
  }
  return 0;
}
