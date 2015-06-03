#include <masks/messages.hh>

namespace master
{
  using namespace network;
  using namespace boost::asio;
  using namespace boost::posix_time;

  inline
  Master::Master()
      : server_{get_ipv6(master::conf.hostname), master::conf.port,
                io_service_,
                std::bind(&Master::recv_dispatcher, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&Master::send_dispatcher, this, std::placeholders::_1, std::placeholders::_2)}
  {
    utils::Logger::cout() << "Concurency level = " + std::to_string(master::conf.concurrency);
    utils::Logger::cout() << "Bind port = " + std::to_string(master::conf.port);
  }

  inline
  Master::~Master()
  {
    if (!threads_.empty())
      stop();
  }

  // Creates threads & make them bind the same port defined in the config.
  inline bool
  Master::run() // Throws
  {
    if (!server_.is_running())
    {
      stop();
      return false;
    }
    // Creating (concurrent_threads) threads
    unsigned concurrency = master::conf.concurrency;
    for (unsigned i = 0; i < concurrency; ++i)
    {
      threads_.emplace_back(
          std::thread([i, this]()
          {
            std::ostringstream s;
            s << "Thread " << i + 1 << " launched (id="
                           << std::this_thread::get_id() << ")!";
            utils::Logger::cout() << s.str();
            io_service_.run();
          })
      );
    }
    return true;
  }

  // Causes the server to stop it's running threads if any.
  inline void
  Master::stop()
  {
    utils::Logger::cout() << "The server is going to stop...";
    server_.stop();

    /// Join all threads
    std::for_each(threads_.begin(), threads_.end(),
        [](std::thread &t)
        {
          std::ostringstream id;
          id << t.get_id();
          utils::Logger::cout() << "Stopping thread " + id.str() + "...";;
          t.join();
          utils::Logger::cout() << "Done stopping thread " + id.str() + "!";
        }
    );
  }

  // When CTRL+C is typed, we call master::stop();
  inline void
  Master::catch_stop()
  {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = [](int s)
    {
      utils::Logger::cout() << "Master received signal " + std::to_string(s)
                               + "...";
    };
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    pause();

    stop();

    utils::Logger::cout() << "Master: Bye bye!";
  }

  // Handle the session after filling the buffer
  // Errors are defined in the ressources/errors file.
  inline error_code
  Master::recv_dispatcher(Packet packet, Session& session)
  {
    if (packet.size_get() < 1)
      return 1;



    switch (packet.fromto_get())
    {
      case c_m::fromto:
        switch (packet.what_get())
        {
          case c_m::error_w:
            return 10001; // Error
          case c_m::up_req_w:
            return cm_up_req(packet, session);
          case c_m::down_req_w:
            return cm_down_req(packet, session);
          case c_m::del_req_w:
            return cm_del_req(packet, session);
          default:
            return 10001;
        }
      case s_m::fromto:
        switch (packet.what_get())
        {
          case s_m::error_w:
            return 10501; // Error
          case s_m::del_ack_w:
            return sm_del_ack(packet, session);
          case s_m::part_ack_w:
            return sm_part_ack(packet, session);
          default:
            return 10501;
        }
      case m_m::fromto:
        switch (packet.what_get())
        {
          default:
            return 10601; // Error
        }
      default:
        return 1; // Error
    }
  }

  inline error_code
  Master::send_dispatcher(Packet packet, Session& session)
  {
    (void)packet;
    (void)session;
    return 0;
  }
}
