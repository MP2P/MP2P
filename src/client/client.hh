#ifndef MP2P_CLIENT
# define MP2P_CLIENT

# include <libconfig.h++>
# include <libtools.hh>

using namespace "network";

class Client
{
private:
  //std::unique_ptr<libconfig::Config> config_; // FIXME: Useless?

  unsigned port_;
  std::string host_;

  io_service io_service_; // Does not need instantiation
  ip::tcp::socket socket_;

  std::unique_ptr <Error> handle(Session &session);

  void send(Session &session);

public:
  Client(std::unique_ptr <libconfig::Config> &&config);

  ~Client();

  // Creates threads & make them bind the same port defined in config.
  void run();

  // Causes the server to stop it's running threads if any.
  void stop();
};

#endif /* MP2P_CLIENT */
