#include <iostream>
#include "master.hh"

Master::Master(std::unique_ptr<libconfig::Config>&& config)
  : //config_{std::move(config)},
  port_{utils::get_port(config)},
  server_{io_service_, port_,
    std::bind(&Master::handle, this, std::placeholders::_1)}
{
  concurent_threads_ = utils::get_concurent_threads(config);
  std::cout << "Concurency level = " << concurent_threads_ << std::endl;
  std::cout << "Bind port = " << port_ << std::endl;
}

Master::~Master()
{
  if (!threads_.empty())
    stop();
}

// Creates threads & make them bind the same port defined in the config.
void Master::run()
{
  // Creating (concurent_threads) threads
  for (unsigned i = 0; i < concurent_threads_; ++i)
  {
    threads_.emplace_front(
      std::thread(
        [i, this]()
        {
          // Using a mutex to avoid printing asynchronously.
          std::cout << "Thread " << i + 1 << " launched (id=" << std::this_thread::get_id() << ")!" << std::endl;
          io_service_.run();
        }
      )
    );
  }
}

// Causes the server to stop it's running threads if any.
void Master::stop()
{
  std::cout << "The server is going to stop..." << std::endl;
  server_.stop();

  /// Join all threads
  std::for_each(threads_.begin(), threads_.end(),
      [](std::thread& t)
      {
      std::cout << "Stopping thread " << t.get_id() << "..." << std::flush;
      t.join();
      std::cout << " Done!" << std::endl;
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
    std::cout << std::endl << "Master received signal " << s << "..." << std::endl;
  };
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  pause();

  stop();

  std::cout << "Master: Bye bye!" << std::endl;
}


// Handle the session after filling the buffer
std::unique_ptr<Error> Master::handle(Session& session)
{
  std::cout << "Master handle (tid=" << std::this_thread::get_id() << ")" << std::endl;

  // Create and get the Packet object from the session (buff_ & length_)
  Packet packet = session.get_packet();


  if (packet.size_get() < 3)
    return std::make_unique<Error>(Error::ErrorType::failure);

  std::cout << packet;

  switch (packet.fromto_get())
  {
    case FromTo::C_to_M:
      return Handle_CM(packet, session); // If the Packet is from a client
    case FromTo::S_to_M:
      return Handle_SM(packet, session); // If the Packet is from a storage
    case FromTo::M_to_M:
      return Handle_MM(packet, session); // If the Packet is from a master
    default:
      return std::make_unique<Error>(Error::ErrorType::failure); // Else failure
  }
/*

  if (packet.message_get() == "SEND")
  {
    std::cout << "Received SEND message!" << std::endl;
    std::string message("TESTING SENDING");
    Packet p{1, 2, message};
    session.send(p);
    return KeepAlive::Live;
  }
*/
  // FIXME : Close me maybe
  return std::make_unique<Error>(Error::ErrorType::failure);
}
