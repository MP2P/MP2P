#include <utils.hh>

#include <chrono>

namespace utils
{
  Active::Active()
    : done_{false},
      thd_{[this]{ Run(); }}
  {
  }

  Active::~Active()
  {
    Send([&]{ done_ = true; });;
    thd_.join();
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
        msg();
      else
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  Logger::Logger(std::ostream &stream)
    : stream_{stream}
  {}

  Logger& Logger::cout()
  {
    static Logger instance(std::cout);
    return instance;
  }

  Logger& Logger::cerr()
  {
    static Logger instance(std::cerr);
    return instance;
  }
}
