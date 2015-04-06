#include <utils.hh>

namespace utils
{
  //void print_debug(const std::string &info)
  //{
  //  if (DEBUG)
  //    std::cout << info << std::endl;
  //}

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

  //void Logger::Print(std::string message)
  //template <typename T> void Logger::Print(T t)
  //{
    //a_.Send( [=]
      //{
        //time_t now = time(0);
        //struct tm tstruct;
        //char buf[80];
        //tstruct = *localtime(&now);
        ////strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        //strftime(buf, sizeof(buf), "%X", &tstruct);
        //g(stream_);
        //stream_ << "<" << buf << "> ";
        //w(stream_);
        //stream_ << t << std::endl;
      //} );
  //}

//void print(std::ostream &out, std::mutex &wmutex, const std::string &msg)
//{
//time_t now = time(0);
//struct tm tstruct;
//char buf[80];
//tstruct = *localtime(&now);
////strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
//strftime(buf, sizeof(buf), "%X", &tstruct);
//wmutex.lock();
//g(out);
//out << "<" << buf << "> ";
//w(out);
//out << msg << std::endl;
//wmutex.unlock();
//}
}
