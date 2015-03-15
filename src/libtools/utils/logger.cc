#include <utils.hh>

namespace utils
{
  void print_debug(const std::string &info)
  {
    if (debug)
      std::cout << info << std::endl;
  }

  void print(std::ostream &out, std::mutex &wmutex, const std::string &msg)
  {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    //strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    wmutex.lock();
    g(out);
    out << "<" << buf << "> ";
    w(out);
    out << msg << std::endl;
    wmutex.unlock();
  }
}
