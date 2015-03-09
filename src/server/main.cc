#include <iostream>
#include <memory>

#include <libtools.hh>
#include "master.hh"

int main()
{
  if (!utils::is_system_ok())
    return 1;
 
  std::unique_ptr<libconfig::Config> config =
    utils::get_config("../config/server.conf");

  if (!config)
    return 1;

  Master master(std::move(config));

  master.run();
  master.catch_stop();
}
