#ifndef MP2P_LIBTOOLS
# define MP2P_LIBTOOLS
# include <memory>
# include <forward_list>
# include <libconfig.h++>
# include <thread>
# include <boost/asio.hpp>
# include <mutex>

# include <libtools.hh>

using namespace boost::asio;

namespace network
{
  class Packet
  {
    private:
      size_t size_;
      char fromto_;
      char what_;
      std::string message_;

    public:
      Packet(size_t size, char fromto, char what, std::string message);
      ~Packet();

      size_t size_get();
      char fromto_get();
      char what_get();
      std::string& message_get();
      std::string serialize();
  };

  class Session
  {
    private:
      ip::tcp::socket socket_;
      streambuf buff_;
      unsigned length_;
      std::function<void(Session&)> handler_;

    public:
      Session(ip::tcp::socket&& socket, std::function<void(Session&)> handler);
      ip::tcp::socket& socket_get();
      streambuf& buff_get();
      unsigned length_get();
      void recieve();
  };

  class Server
  {
    private:
      ip::tcp::acceptor acceptor_;
      ip::tcp::socket socket_;
      std::function<void(Session&)> handler_;
      std::vector<std::shared_ptr<Session>> sessions_;

    public:
      Server(io_service& io_service,
             const unsigned port,
             std::function<void(Session&)> handler);
      ~Server();

      boost::asio::streambuf& buff_get();
      void listen(); // Listen to accept connections
  };

  class Master
  {
    private:
      std::unique_ptr<libconfig::Config> config_; // FIXME: Useless?
      std::forward_list<std::thread> threads_;
      unsigned port_;
      unsigned concurent_threads_;
      io_service io_service_; // Does not need instantiation
      Server server_;
      std::mutex w_mutex_; // Just for testing purposes.

      void handle(Session& session);

    public:
      Master(std::unique_ptr<libconfig::Config>&& config);
      ~Master();

      // Creates threads & make them bind the same port defined in config.
      void run();

      // Causes the server to stop it's running threads if any.
      void stop();
  };
}

namespace files
{
}

namespace utils
{
  void print_debug(const std::string& info);

  void print(std::ostream& out, std::mutex& wmutex, const std::string& msg);

  /// Creates a Config instance with the given path, if file does not exists,
  /// or parse failed it returns NULL
  std::unique_ptr<libconfig::Config> get_config(const std::string& path);

  unsigned get_port(std::unique_ptr<libconfig::Config>& config);
  unsigned get_concurent_threads(std::unique_ptr<libconfig::Config>& config);

  bool is_system_ok();
}

#endif /* MP2P_LIBTOOLS */
