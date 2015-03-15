#ifndef MP2P_SERVER
# define MP2P_SERVER

# include <forward_list>
# include <libconfig.h++>
# include <thread>
# include <boost/asio.hpp>
# include <libtools.hh>

using namespace boost::asio;
using namespace network;

class Master
{
private:
  //std::unique_ptr<libconfig::Config> config_; FIXME: Useless?


  std::forward_list<std::thread> threads_;
  unsigned port_;
  unsigned concurent_threads_;
  time_duration timeout_;

  io_service io_service_; // Does not need instantiation
  network::Server server_;

  std::unique_ptr<Error> handle(Session &session);

  // Causes the server to stop it's running threads if any.
  void stop();

public:
  Master(std::unique_ptr<libconfig::Config> &&config);

  ~Master();

  // Creates threads & make them bind the same port defined in config.
  bool run();

  // Catch a CTRL+C / CTRL+D signal, call Master::stop(); and exit.
  void catch_stop();
};

std::unique_ptr<Error> Handle_CM(Packet & packet, Session & session);
std::unique_ptr<Error> Handle_SM(Packet & packet, Session & session);
std::unique_ptr<Error> Handle_MM(Packet & packet, Session & session);

#endif /* MP2P_SERVER */
