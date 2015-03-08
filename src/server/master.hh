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
    std::unique_ptr<libconfig::Config> config_; // FIXME: Useless?


    std::forward_list<std::thread> threads_;
    unsigned port_;
    unsigned concurent_threads_;

    io_service io_service_; // Does not need instantiation
    network::Server server_;

    KeepAlive handle(Session& session);

    // Causes the server to stop it's running threads if any.
    void stop();
  public:
    Master(std::unique_ptr<libconfig::Config>&& config);
    ~Master();

    // Creates threads & make them bind the same port defined in config.
    void run();

    // Catch a CTRL+C / CTRL+D signal, call Master::stop(); and exit.
    void catch_stop();
};

#endif /* MP2P_SERVER */
