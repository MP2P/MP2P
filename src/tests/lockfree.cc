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
      //std::cout << "Destroying the Active..." << std::endl;
      Send([&]{ done_ = true; });;
      thd_->join();
      //std::cout << "Active destroyed!" << std::endl;
    }

    void Send(Message m)
    {
      //m_.lock();
      //std::cout << "Pushing an element..." << std::endl;
      mq_.enqueue(m);
      //std::cout << "Done pushing an element!" << std::endl;
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
          //std::cout << "Poping..." << std::endl;
          msg();
          //std::cout << "Poped!" << std::endl;
        }
        else
        {
          //std::cout << "Sleeping..." << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
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
         //std::cout << "Preparing printing..." << std::endl;
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
         //std::cout << "Done printing!" << std::endl;
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
  {
    //std::cout << "Requesting print " + std::to_string(i) + "..." << std::endl;
    //lock_free_logger.Print("This is a lock-free printer message " + std::to_string(i) + " from thread " + std::to_string(thread_nb) + "!" );
    lock_free_logger.Print("This is a lock-free printer message from thread " + std::to_string(thread_nb) + "!" );
    //std::cout << "Done requesting print " + std::to_string(i) + "!" << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //classic_printer(std::cout, m, "This is a naive printer message from thread " + std::to_string(thread_nb) + "!");
}
