#include <iostream>
#include <libtools.hh>
#include "client.hh"

int main()
{
  if (!utils::is_system_ok())
    return 1;

  std::unique_ptr <libconfig::Config> config =
      utils::get_config("../config/server.conf");

  if (!config)
    return 1;

  try
  {
    Client client(std::move(config));

    client.run();
    client.stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Client failed : " << e.what() << std::endl;
  }
}
