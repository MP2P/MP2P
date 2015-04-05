#include <utils.hh>
#include <network.hh>

#include <sys/socket.h>
#include <netinet/in.h>

namespace utils
{
  bool is_system_ok()
  {
    int sock = socket(PF_INET6, SOCK_STREAM, 0);

    int val = 0;
    unsigned len = 0;
    if (::getsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &val, &len))
    {
      std::cerr << "Fatal error: IPV6_V6ONLY must be false." << std::endl;
      return false;
    }
    return true;
  }

  bool init()
  {
    //std::cout << "Initialization..." << std::endl;
    utils::Logger::cout() << "Initialization...";
    if (!is_system_ok())
      return false;

    utils::Conf& cfg = utils::Conf::get_instance();
    if (!cfg.update_conf("../config/server.conf"))
      return false;

    if (!network::Error::update_conf("../ressources/errors"))
      return false;

    return true;
  }
}
