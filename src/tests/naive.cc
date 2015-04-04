#include "main.hh"

std::mutex m;

void naive_printer(std::ostream &out, std::mutex &wmutex, const std::string &msg)
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

void naive_caller(unsigned nb_messages, unsigned thread_nb)
{
  for (unsigned i = 0; i < nb_messages; i++)
    naive_printer(std::cout, m, "This is a naive printer message from thread " + std::to_string(thread_nb) + "!");
}
