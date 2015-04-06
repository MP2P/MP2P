#include "main.hh"

class Active2
{
  public:
    typedef std::function<void()> Message;

    Active2()
      : done_{false}
    {
      thd_ = std::unique_ptr<std::thread>(new std::thread( [=]{this->Run();} ));
    }

    ~Active2()
    {
      Send([&]{ done_ = true; });;
      thd_->join();
    }

    void Send(Message m)
    {
      m_.lock();
      //std::cerr << "Size before pushing: " + std::to_string(mq_.size()) << std::endl;
      mq_.push(m);
      m_.unlock();
    }

  private:
    Active2(const Active2&);        // no copying
    void operator=(const Active2&);// no copying

    bool done_;
    std::queue<Message> mq_;
    std::mutex m_; //To synchronize the writes in the queue
    std::unique_ptr<std::thread> thd_;

    void Run()
    {
      while(!done_)
      {
        if (mq_.empty())
        {
          //std::cout << "Sleeping..." << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else
        {
          //std::cout << "Poping..." << std::endl;
          Message msg = mq_.front();
          msg();
          mq_.pop();
        }
      }
    }
};

class Logger2
{
  public:
    Logger2(std::ostream &stream = std::cout)
      : stream_{stream}
    {}

    void Print(std::string message)
    {
      a.Send( [=]
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
         stream_ << message << std::endl;
       } );
    }

  private:
    std::ostream &stream_;
    Active2 a;
};

Logger2 l;

void classic_caller(unsigned nb_messages, unsigned thread_nb)
{
  for (unsigned i = 0; i < nb_messages; i++)
    l.Print("This is a classic printer message from thread " + std::to_string(thread_nb) + "!" );
  //classic_printer(std::cout, m, "This is a naive printer message from thread " + std::to_string(thread_nb) + "!");
}
