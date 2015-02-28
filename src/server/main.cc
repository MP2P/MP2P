#include <iostream>
#include <memory>

#include <libtools.hh>

int main()
{
  if (!utils::is_system_ok())
    return 1;

  std::cout << "Here is the server!" << std::endl;

  std::unique_ptr<libconfig::Config> config =
    utils::get_config("../config/server.conf");

  if (!config)
    return 1;

  network::Master master(std::move(config));

  master.run();
  master.stop();
}
