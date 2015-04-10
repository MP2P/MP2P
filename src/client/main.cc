#include <utils.hh>
#include "client.hh"

#include <iostream>

int main(int argc, const char *argv[])
{
  try
  {
    if (argc <= 1)
      throw std::logic_error("USAGE: ./client filename");

    utils::init();

    // Prepare file
    files::File file(argv[1]);

    // Prepare socket
    std::ostringstream port;
    port << utils::Conf::get_instance().get_port();
    const auto& host = utils::Conf::get_instance().get_host();
    utils::Logger::cout() << "Endpoint host = " << host;
    utils::Logger::cout() << "Endpoint port = " << port.str();

    Client client{host, port.str()};

    client.run();

    client.send_file(file);

    client.stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Client failed : " << e.what() << std::endl;
  }
}
