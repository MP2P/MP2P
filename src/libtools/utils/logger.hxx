#pragma once

#include <utils.hh>

#include <iostream>
#include <chrono>
#include <iomanip>

namespace utils
{
  template <typename T>
  void clog(T t)
  {
    std::clog << color::g << "<" << "> " << color::w << t << std::endl;
  }

  template <typename T>
  void Logger::Print(T t)
  {
    a_.Send( [=]
      {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        color::g(stream_);
        stream_ << "<"
                << std::put_time(std::localtime(&now_c), "%T")
                << "> ";
        color::w(stream_);
        stream_ << t << std::endl;
      }
    );
  }

  template <typename T>
  Logger& Logger::operator<<(T t)
  {
    Print(t);
    return *this;
  }
}
