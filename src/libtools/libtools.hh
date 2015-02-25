#ifndef MP2P_LIBTOOLS
# define MP2P_LIBTOOLS
# include <memory>
# include <forward_list>
# include <libconfig.h++>
# include <thread>

# include <libtools.hh>


void start();

namespace network
{
  enum Node
  {
    CLIENT = 0,
    MASTER = 1,
    STORAGE = 2,
    ZEUS = 3
  };

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

  void send_packet(Packet& packet);

  /// Each Thread instanciates a Server
  class Server
  {
  };

  class Master
  {
    private:
      std::unique_ptr<libconfig::Config> config_;
      std::forward_list<std::thread> threads_;
      unsigned port_;
      unsigned concurent_threads_;
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

}

#endif /* MP2P_LIBTOOLS */
