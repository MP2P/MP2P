#include <utils.hh>
#include <network.hh>


namespace utils
{
  void check_system()
  {
    int sock = socket(PF_INET6, SOCK_STREAM, 0);

    int val = 0;
    unsigned len = 0;
    if (::getsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &val, &len))
      throw std::logic_error("Fatal error: IPV6_V6ONLY must be false.");
  }
//
//  void init()
//  {
//    utils::Logger::cout() << "Initialization...";
//    check_system();
//
////    utils::Conf& cfg = utils::Conf::get_instance();
////    if (!cfg.initialize(config_path))
////      throw std::logic_error("Config file failed to init");
//
//    // Specify all error messages here
////    network::Error::errors.emplace(id, msg);
//  }
}
