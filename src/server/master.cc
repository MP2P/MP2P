#include <iostream>

#include <utils.hh>
#include "master.hh"

Master::Master()
    : server_{io_service_, std::bind(&Master::handle, this, std::placeholders::_1)}
{
  unsigned concurrency = utils::Conf::get_instance().get_concurrency();
  unsigned port = utils::Conf::get_instance().get_port();
  //std::cout << "Concurency level = " << concurrency << std::endl;
  utils::Logger::cout() << "Concurency level = " << concurrency;
  //std::cout << "Bind port = " << port << std::endl;
  utils::Logger::cout() << "Bind port = " << port;
}

Master::~Master()
{
  if (!threads_.empty())
    stop();
}

// Creates threads & make them bind the same port defined in the config.
bool Master::run()
{
  if (!server_.is_running())
  {
    stop();
    return false;
  }
  // Creating (concurent_threads) threads
  unsigned concurrency = utils::Conf::get_instance().get_concurrency();
  for (unsigned i = 0; i < concurrency; ++i)
  {
    threads_.emplace_front(
        std::thread(
            [i, this]()
            {
              // Using a mutex to avoid printing asynchronously.
              //std::cout << "Thread " << i + 1 << " launched "
                  //"(id=" << std::this_thread::get_id() << ")!" << std::endl;
              utils::Logger::cout() << "Thread " + std::to_string(i + 1) + " launched "
                  "(id=" << std::this_thread::get_id() << ")!";
              io_service_.run();
            }
        )
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
        //std::cout << "Stopping thread " << t.get_id() << "..." << std::flush;
        utils::Logger::cout() << "Stopping thread " << t.get_id() << "...";
        t.join();
        //std::cout << " Done!" << std::endl;
        utils::Logger::cout() << "Done stopping thread " << t.get_id() << "!";
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
    //std::cout << std::endl << "Master received signal " << s << "..." << std::endl;
    utils::Logger::cout() << "\nMaster received signal " << s << "...";
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
error_code Master::handle(Session & session)
{
  //std::cout << "Master handle (tid=" << std::this_thread::get_id() << ")" << std::endl;
  utils::Logger::cout() << "Master handle (tid=" << std::this_thread::get_id() << ").";

  // Create and get the Packet object from the session (buff_ & length_)
  Packet packet = session.get_packet();


  if (packet.get_size() < 3)
    return 1;

  //std::cout << packet;
  utils::Logger::cout() << packet;

  switch (packet.get_fromto())
  {
    case FromTo::C_to_M: // Code=0
      switch (packet.get_what())
      {
        case 0:
          return 10001; // Error
        case 1:
          return CM_callback_may_i_upload_a_file(packet, session);
        case 2:
          return CM_callback_may_i_download_this_file(packet, session);
        case 3:
          return CM_callback_can_you_delete_this_file(packet, session);
        default:
          return 10001;
      }
    case FromTo::S_to_M: // Code=5
      switch (packet.get_what())
      {
        case 0:
          return 10501; // Error
        case 1:
          return SM_callback_part_deletion_succeded(packet, session);
        case 2:
          return SM_callback_part_received(packet, session);
        default:
          return 10501;
      }
    case FromTo::M_to_M: // Code=6
      return 10601; // Error
    default:
      return 1; // Error
  }
}
