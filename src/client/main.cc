#include <utils.hh>
#include "client.hh"

int main(int argc, const char *argv[])
{
  using namespace client; // Use this namespace inside the function only

  try
  {
    parse_options(argc, argv);
  }
  catch (int i)
  {
    return i;
  }
  try
  {
    utils::init();

    // Prepare socket
    std::ostringstream port;
    port << client::conf.master_port;
    const auto& host = client::conf.master_hostname;
    utils::Logger::cout() << "Endpoint host = " + host;
    utils::Logger::cout() << "Endpoint port = " + port.str();

    Client client{host, port.str()};

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
    utils::Logger::cerr() << "Client failed : " + std::string(e.what());
  }
}
