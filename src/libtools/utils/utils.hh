#pragma once

#include <ostream>
#include <string>
#include <mutex>
#include <libconfig.h++>
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;

const bool debug = true;

namespace utils
{
  /*--------.
  | init.cc |
  `--------*/
  bool is_system_ok();
  bool init();

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

  void print(std::ostream &out, std::mutex &wmutex, const std::string &msg);
}
