#include <utils.hh>
#include "client.hh"

int main(int argc, const char *argv[])
{
  using namespace client; // Use this namespace inside the function only

  conf config;
  try
  {
    parse_options(argc, argv, config);
  }
  catch (int i)
  {
    return i;
  }
  try
  {
    utils::init(config.config_path);

    // Prepare socket
    std::ostringstream port;
    port << utils::Conf::get_instance().port_get();
    const auto& host = utils::Conf::get_instance().host_get();
    utils::Logger::cout() << "Endpoint host = " + host;
    utils::Logger::cout() << "Endpoint port = " + port.str();

    Client client{host, port.str()};

    client.run();

    if (config.action == action::upload)
    {
      // Prepare file
      files::File file(config.file_path);

      client.send_file(file, config.redundancy);
    }

    client.stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Client failed : " + std::string(e.what());
  }
}
