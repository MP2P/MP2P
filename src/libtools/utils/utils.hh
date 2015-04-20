#pragma once

#include <ostream>
#include <string>
#include <mutex>
#include <libconfig.h++>
#include <thread>
#include "concurentqueue.hh"
#include "boost/date_time/posix_time/posix_time.hpp"

//#define DEBUG true

using error_code = uint16_t;

namespace utils
{
  /*--------.
  | init.cc |
  `--------*/
  // Functions throwing exceptions if something goes bad
  void check_system();
  void init();


  /*---------.
  | color.cc |
  `---------*/
  std::ostream& w(std::ostream& o = std::cout);
  std::ostream& r(std::ostream& o = std::cout);
  std::ostream& b(std::ostream& o = std::cout);
  std::ostream& c(std::ostream& o = std::cout);
  std::ostream& g(std::ostream& o = std::cout);
  std::ostream& y(std::ostream& o = std::cout);
  std::ostream& p(std::ostream& o = std::cout);


  /*----------.
  | config.cc |
  `----------*/
  class Conf
  {
  private:
    Conf() = default;
    Conf(Conf const& ) = delete;
    void operator=(Conf const& ) = delete;

    std::string host_;
    unsigned port_;
    unsigned concurrent_threads_;
    boost::posix_time::time_duration timeout_;
    std::string DBhost_;
    unsigned DBport_;
    std::string DBpassword_;
    std::string DBbucket_;

  public:
    // Singleton
    static Conf& get_instance();

    bool initialize(const std::string& config_path);

    std::string host_get() const;
    unsigned port_get() const;
    unsigned concurrency_get() const;
    boost::posix_time::time_duration timeout_get() const;
    std::string DBhost_get() const;
    unsigned DBport_get() const;
    std::string DBpassword_get() const;
    std::string DBbucket_get() const;
  };


  /*----------.
  | logger.cc |
  `----------*/
  void print_debug(const std::string& info);

  class Active
  {
    public:
      typedef std::function<void()> Message;

      Active();
      ~Active();

      void Send(Message m);

    private:
      Active(const Active&) = delete;
      void operator=(const Active&) = delete;

      void Run();

      bool done_;
      moodycamel::ConcurrentQueue<Message> mq_;
      std::unique_ptr<std::thread> thd_;
  };

  class Logger
  {
    public:
      Logger(std::ostream& stream = std::cout);

      static Logger& cout();
      static Logger& cerr();

      template <typename T>
      void Print(T t);

      template <typename T>
      Logger& operator<<(T t);

    private:
      std::ostream& stream_;
      Active a_;
  };
  //void print(std::ostream& out, std::mutex& wmutex, const std::string& msg);
}

#include "config.hxx"
#include "logger.hxx"
