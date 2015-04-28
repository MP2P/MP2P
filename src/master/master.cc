#include <iostream>

#include <utils.hh>
#include "master.hh"

using namespace network;
using namespace boost::asio;
using namespace boost::posix_time;

Master::Master()
    : server_{io_service_,
              std::bind(&Master::handle, this, std::placeholders::_1)}
{
  unsigned concurrency = utils::Conf::get_instance().concurrency_get();
  unsigned port = utils::Conf::get_instance().port_get();

  utils::Logger::cout() << "Concurency level = " + std::to_string(concurrency);
  utils::Logger::cout() << "Bind port = " + std::to_string(port);
}

Master::~Master()
{
  if (!threads_.empty())
    stop();
}

// Creates threads & make them bind the same port defined in the config.
bool Master::run() // Throws
{
  if (!server_.is_running())
  {
    stop();
    return false;
  }
  // Creating (concurrent_threads) threads
  unsigned concurrency = utils::Conf::get_instance().concurrency_get();
  for (unsigned i = 0; i < concurrency; ++i)
  {
    threads_.emplace_front(
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
void Master::stop()
{
  //std::cout << "The server is going to stop..." << std::endl;
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
        //std::cout << " Done!" << std::endl;
        utils::Logger::cout() << "Done stopping thread " + id.str() + "!";
      }
  );

  // Delete all threads
  while (!threads_.empty())
    threads_.pop_front();
}

// When CTRL+C is typed, we call master::stop();
void Master::catch_stop()
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

  //std::cout << "Master: Bye bye!" << std::endl;
  utils::Logger::cout() << "Master: Bye bye!";
}


// Handle the session after filling the buffer
// Errors are defined in the ressources/errors file.
error_code Master::handle(Session& session)
{
  std::ostringstream s;
  s << std::this_thread::get_id();
  utils::Logger::cout() << "Master handle (tid=" + s.str() + ").";

  // Create and get the Packet object from the session (buff_ & length_)
  Packet packet = session.get_packet();

  if (packet.size_get() < 1)
    return 1;

  switch (packet.fromto_get())
  {
    case FromTo::C_to_M: // Code=0
      switch (packet.what_get())
      {
        case 0:
          return 10001; // Error
        case 1:
          return cm_up_req(packet, session);
        case 2:
          return cm_down_req(packet, session);
        case 3:
          return cm_del_req(packet, session);
        default:
          return 10001;
      }
    case FromTo::S_to_M: // Code=5
      switch (packet.what_get())
      {
        case 0:
          return 10501; // Error
        case 1:
          return sm_del_ack(packet, session);
        case 2:
          return sm_part_ack(packet, session);
        default:
          return 10501;
      }
    case FromTo::M_to_M: // Code=6
      return 10601; // Error
    default:
      return 1; // Error
  }
}
