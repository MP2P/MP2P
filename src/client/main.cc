#include <utils.hh>
#include "client.hh"

#include <iostream>

int main(int argc, const char *argv[])
{
  if (!utils::is_system_ok())
    return 1;

  utils::Conf& cfg = utils::Conf::get_instance();
  if (!cfg.update_conf("../config/server.conf"))
    return 1;

  try
  {
    if (argc <= 1)
      throw std::logic_error("USAGE: ./client filename");

    files::File file(argv[1]);

    Client client{};

    client.run();

    client.send_file(file);

    client.stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Client failed : " << e.what() << std::endl;
  }
}
