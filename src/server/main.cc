#include <utils.hh>
#include "master.hh"

#include <ostream>

int main()
{
  if (!utils::is_system_ok())
    return 1;

  utils::Conf& cfg = utils::Conf::get_instance();
  if (!cfg.update_conf("../config/server.conf"))
    return 1;

  try
  {
    Master master{};

    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Server failed : " << e.what() << std::endl;
  }
}
