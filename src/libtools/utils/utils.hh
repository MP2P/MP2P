#pragma once

#include <iostream>
#include <string>
#include <thread>
#include "concurentqueue.hh"

//#define DEBUG true

namespace infos
{
  // Global variables defined in infos.cc
  extern const std::string version;
  extern const std::string copyright;
}

namespace utils
{
  /*-----.
  | Init |
  `-----*/

  // Functions throwing exceptions if something goes bad
  void check_system();
  void init();

  /*------.
  | Color |
  `------*/

  namespace color
  {
    std::ostream& w(std::ostream& o = std::cout);
    std::ostream& r(std::ostream& o = std::cout);
    std::ostream& b(std::ostream& o = std::cout);
    std::ostream& c(std::ostream& o = std::cout);
    std::ostream& g(std::ostream& o = std::cout);
    std::ostream& y(std::ostream& o = std::cout);
    std::ostream& p(std::ostream& o = std::cout);
  }

  /*-------.
  | Logger |
  `-------*/

  class Active
  {
    public:
      using Message = std::function<void()>;

      Active();
      ~Active();
      Active(const Active&) = delete;
      void operator=(const Active&) = delete;

      void Send(Message m);

    private:
      void Run();

      bool done_;
      moodycamel::ConcurrentQueue<Message> mq_;
      std::thread thd_;
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

  namespace misc
  {
    template <class C, typename S>
    class Separator
    {
    private:
      const C& container_;
      const S& separator_;
    public:
      Separator(const C& c, const S& s);
      std::ostream& operator()(std::ostream& o) const;
    };

    template <class C, typename S>
    Separator<C, S>
    separate(const C& c, const S& s);

    template <class C, typename S>
    inline std::ostream&
    operator<<(std::ostream& o, const Separator<C, S>& s);

    template <typename T>
    std::string string_from(T value);

    std::string string_from(const unsigned char* value, size_t size);
    std::string string_from(const char* value, size_t size);
  }
}

#include "misc.hxx"
#include "logger.hxx"
