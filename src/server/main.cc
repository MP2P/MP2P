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

  std::unique_ptr<network::Master> master =
    std::make_unique<network::Master>(std::move(config));

  master->run();
  master->stop();
}
