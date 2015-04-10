#pragma once

#include <ostream>
#include <string>
#include <mutex>
#include <libconfig.h++>
#include <thread>
#include "concurentqueue.hh"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;

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
  std::ostream &w(std::ostream &o = std::cout);
  std::ostream &r(std::ostream &o = std::cout);
  std::ostream &b(std::ostream &o = std::cout);
  std::ostream &c(std::ostream &o = std::cout);
  std::ostream &g(std::ostream &o = std::cout);
  std::ostream &y(std::ostream &o = std::cout);
  std::ostream &p(std::ostream &o = std::cout);


  /*----------.
  | config.cc |
  `----------*/
  // -> Singleton Class
  class Conf
  {
  private:
    Conf();
    Conf(Conf const &) = delete;
    void operator=(Conf const &) = delete;

    std::string host_;
    unsigned port_;
    unsigned concurrent_threads_;
    time_duration timeout_;
  public:
    static Conf &get_instance()
    {
      static Conf instance;
      return instance;
    }

    bool update_conf(const std::string &config_path);

    std::string get_host();
    unsigned get_port();
    unsigned get_concurrency();
    time_duration get_timeout();
  };


  /*----------.
  | logger.cc |
  `----------*/
  void print_debug(const std::string &info);

  class Active
  {
    public:
      typedef std::function<void()> Message;

      Active();
      ~Active();

      void Send(Message m);

    private:
      Active(const Active&);        // no copying
      void operator=(const Active&);// no copying

      void Run();

      bool done_;
      moodycamel::ConcurrentQueue<Message> mq_;
      std::unique_ptr<std::thread> thd_;
  };

  class Logger
  {
    public:
      Logger(std::ostream &stream = std::cout);

      static Logger &cout()
      {
        static Logger instance;
        return instance;
      }
      static Logger &cerr()
      {
        static Logger instance(std::cerr);
        return instance;
      }
      template <typename T> void Print(T t)
      {
        a_.Send( [=]
          {
            time_t now = time(0);
            struct tm tstruct;
            char buf[80];
            tstruct = *localtime(&now);
            //strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
            strftime(buf, sizeof(buf), "%X", &tstruct);
            g(stream_);
            stream_ << "<" << buf << "> ";
            w(stream_);
            stream_ << t << std::endl;
          } );
      }
      //Logger &operator<<(std::string s)
      //{
      //Print(s);
      //return this;
      //}
      template <typename T> Logger &operator<<(T t)
      {
        Print(t);
        return *this;
      }

    private:
      std::ostream &stream_;
      Active a_;
  };
  //void print(std::ostream &out, std::mutex &wmutex, const std::string &msg);
}
