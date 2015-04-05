//#include <boost/lockfree/queue.hpp>
#include "concurentqueue.hh"
#include "main.hh"

class Active
{
  public:
    typedef std::function<void()> Message;

    Active()
      : done_{false}
    {
      thd_ = std::unique_ptr<std::thread>(new std::thread( [=]{this->Run();} ));
    }

    ~Active()
    {
      Send([&]{ done_ = true; });;
      thd_->join();
    }

    void Send(Message m)
    {
      //m_.lock();
      //std::cerr << "Size before pushing: " + std::to_string(mq_.size()) << std::endl;
      mq_.enqueue(m);
      //m_.unlock();
    }

  private:
    Active(const Active&);        // no copying
    void operator=(const Active&);// no copying

    bool done_;
    //std::queue<Message> mq_;
    //boost::lockfree::queue<Message> mq_;
    moodycamel::ConcurrentQueue<Message> mq_;
    //std::mutex m_; //To synchronize the writes in the queue
    std::unique_ptr<std::thread> thd_;

    void Run()
    {
      while(!done_)
      {
        //if (mq_.empty())
          //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //else
        //{
          //Message msg = mq_.front();
          //msg();
          //mq_.pop();
        //}
        Message msg;
        if (mq_.try_dequeue(msg))
        {
          msg();
        }
        else
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
};

class Logger
{
  public:
    Logger(std::ostream &stream = std::cout)
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
    Active a;
};

Logger lock_free_logger;

void lock_free_caller(unsigned nb_messages, unsigned thread_nb)
{
  for (unsigned i = 0; i < nb_messages; i++)
    lock_free_logger.Print("This is a lock-free printer message from thread " + std::to_string(thread_nb) + "!" );
  //classic_printer(std::cout, m, "This is a naive printer message from thread " + std::to_string(thread_nb) + "!");
}