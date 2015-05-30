#pragma once

#include <utils.hh>

namespace utils
{
  template <typename T>
  void Logger::Print(T t)
  {
    a_.Send( [=]
      {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        //strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        strftime(buf, sizeof(buf), "%X", &tstruct);
        color::g(stream_);
        stream_ << "<" << buf << "> ";
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
