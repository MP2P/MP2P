#ifndef MP2P_LIBTOOLS
# define MP2P_LIBTOOLS
# include <memory>
# include <forward_list>
# include <libconfig.h++>
# include <thread>
# include <boost/asio.hpp>

# include <libtools.hh>

using namespace boost::asio;

void start();

namespace network
{
  class Packet
  {
    private:
      uint32_t size_;
      uint8_t fromto_;
      uint8_t what_;
      std::string message_;

    public:
      Packet(uint32_t size, uint8_t fromto, uint8_t what, std::string message);
      ~Packet();
  };

  void send_packet(Packet& packet); // FIXME
  void handle_master();
  void handle_storage();
  void handle_zeus();
  auto get_handle_fun();

  class Server
  {
    private:
      ip::tcp::acceptor acceptor_;
      ip::tcp::socket socket_;
      std::function<void()> handler_;

      void handle_accept();
    public:
      Server(io_service& io_service, const unsigned port, std::function<void()> handler);
      ~Server();
  };

  class Master
  {
    private:
      std::unique_ptr<libconfig::Config> config_; // FIXME: Useless?
      std::forward_list<std::thread> threads_;
      unsigned port_;
      unsigned concurent_threads_;
      io_service io_service_; // Do not need instantiation
      std::unique_ptr<Server> server_;

      void handle();

    public:
      Master(std::unique_ptr<libconfig::Config>&& config);
      ~Master();

      /// Creates threads & make them bind the same port defined in config.
      void run();

      /// Causes the server to stop it's running threads if any.
      void stop();
  };
}

namespace files
{
}

namespace utils
{
  void print_debug(const std::string& info);

  /// Creates a Config instance with the given path, if file does not exists,
  /// or parse failed it returns NULL
  std::unique_ptr<libconfig::Config> get_config(const std::string& path);

  unsigned get_port(std::unique_ptr<libconfig::Config>& config);
  unsigned get_concurent_threads(std::unique_ptr<libconfig::Config>& config);

  bool is_system_ok();
}

#endif /* MP2P_LIBTOOLS */
