#ifndef MP2P_LIBTOOLS
# define MP2P_LIBTOOLS
# include <memory>
# include <atomic>
# include <forward_list>
# include <libconfig.h++>
# include <thread>
# include <boost/asio.hpp>
# include <mutex>

using namespace boost::asio;

namespace network
{

  enum class KeepAlive
  {
    Live,
    Die
  };

  enum FromTo
  {
    C_to_M = 0,
    M_to_C = 1,
    C_to_S = 2,
    S_to_C = 3,
    M_to_S = 4,
    S_to_M = 5,
    M_to_M = 6,
    S_to_S = 7
  };

  class Packet
  {
    private:
      uint32_t size_;
      uint8_t fromto_;
      uint8_t what_;
      std::string message_;

    public:
      Packet(uint8_t fromto, uint8_t what, std::string message);
      ~Packet();


      // FIXME : unsigned to char. Set to unsigned for testing purposes
      uint32_t size_get();
      uint8_t fromto_get();
      uint8_t what_get();
      std::string& message_get();
      const std::string serialize() const;
      static const Packet deserialize(const std::string& input);
  };

  std::ostream &operator<<(std::ostream& output, const Packet& packet);

  class Session
  {
    private:
      ip::tcp::socket socket_;
      streambuf buff_;
      unsigned length_;
      std::function<KeepAlive(Session&)> handler_;
      std::mutex w_mutex_; // Just for testing purposes.

    public:
      Session(ip::tcp::socket&& socket, std::function<KeepAlive(Session&)> handler);
      ip::tcp::socket& socket_get();
      streambuf& buff_get();
      unsigned length_get();
      std::string get_line();
      Packet get_packet();

      void receive();
      void send(const Packet packet);
  };

  class Server
  {
    private:
      ip::tcp::acceptor acceptor_;
      ip::tcp::socket socket_;
      std::function<KeepAlive(Session&)> handler_;
      std::vector<std::shared_ptr<Session>> sessions_;

    public:
      Server(io_service& io_service,
             const unsigned port,
             std::function<KeepAlive(Session&)> handler);
      ~Server();

      boost::asio::streambuf& buff_get();
      void listen(); // Listen to accept connections
      void stop();
  };
  class Error
  {
      public:
      Error();
      ~Error();

      enum ErrorType
      {
        succes = 0,
        failure = 1
      };

      ErrorType error_type_get();
      std::ostringstream stream_get();

      Error& operator=(const Error& e);
      Error& operator=(ErrorType e);

      // Put the parameter in stream_
      template <typename T> Error& operator<<(const T& t);

      Error& operator<<(std::ostream& (*f)(std::ostream&));
    private:
      ErrorType status_;
      std::ostringstream stream_;
  };
  std::ostream& operator<<(std::ostream& o, const Error& e);
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
