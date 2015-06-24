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
                std::bind(&Master::recv_dispatcher, this,
                          std::placeholders::_1, std::placeholders::_2)}
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
  inline keep_alive
  Master::recv_dispatcher(Packet packet, Session& session)
  {
    if (packet.size_get() < 1)
      return send_error(session, packet, error_code::invalid_packet,
                        "Recieved an invalid packet");

    // FIXME : Customize for handlers. For now, no action is required
    if (packet.what_get() == ack_w)
      return keep_alive::No;

    switch (packet.fromto_get())
    {
      case c_m::fromto:
        switch (packet.what_get())
        {
          case c_m::up_req_w:
            return cm_up_req(packet, session);
          case c_m::down_req_w:
            return cm_down_req(packet, session);
          case c_m::del_req_w:
            return cm_del_req(packet, session);
          default:
            break;
        }
      case s_m::fromto:
        switch (packet.what_get())
        {
          case s_m::part_ack_w:
            return sm_part_ack(packet, session);
          case s_m::id_req_w:
            return sm_id_req(packet, session);
          default:
            break;
        }
      case m_m::fromto:
        switch (packet.what_get())
        {
          default:
            break;
        }
      default:
        break;
    }

    return keep_alive::No; // FIXME
  }

  network::keep_alive send_error(network::Session& session,
                                 const Packet& p,
                                 enum network::error_code error,
                                 std::string msg)
  {
    utils::Logger::cerr() << msg;
    send_ack(session, p, error);
    return keep_alive::No;
  }
}
