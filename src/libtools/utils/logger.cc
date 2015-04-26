#include <utils.hh>

namespace utils
{
  Active::Active()
    : done_{false}
  {
    thd_ = std::unique_ptr<std::thread>(new std::thread( [=]{this->Run();} ));
  }

  Active::~Active()
  {
    Send([&]{ done_ = true; });;
    thd_->join();
  }

  void Active::Send(Message m)
  {
    mq_.enqueue(m);
  }

  void Active::Run()
  {
    while(!done_)
    {
      Message msg;
      if (mq_.try_dequeue(msg))
      {
        msg();
      }
      else
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  Logger::Logger(std::ostream &stream)
    : stream_{stream}
  {}

  Logger& Logger::cout()
  {
    static Logger instance;
    return instance;
  }

  Logger& Logger::cerr()
  {
    static Logger instance(std::cerr);
    return instance;
  }

/*void print(std::ostream &out, std::mutex &wmutex, const std::string &msg)
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
}*/
}
