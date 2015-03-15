#include <utils.hh>
#include "client.hh"

#include <iostream>

int main()
{
  if (!utils::is_system_ok())
    return 1;

  utils::Conf& cfg = utils::Conf::get_instance();
  if (!cfg.update_conf("../config/server.conf"))
    return 1;

  try
  {
    Client client{};

    client.run();
    client.stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Client failed : " << e.what() << std::endl;
  }
}
