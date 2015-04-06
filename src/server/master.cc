#include <iostream>

#include <utils.hh>
#include "master.hh"

Master::Master()
    : server_{io_service_, std::bind(&Master::handle, this, std::placeholders::_1)}
{
  unsigned concurrency = utils::Conf::get_instance().get_concurrency();
  unsigned port = utils::Conf::get_instance().get_port();
  std::cout << "Concurency level = " << concurrency << std::endl;
  std::cout << "Bind port = " << port << std::endl;
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
              std::cout << "Thread " << i + 1 << " launched "
                  "(id=" << std::this_thread::get_id() << ")!" << std::endl;
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
  std::cout << "The server is going to stop..." << std::endl;
  server_.stop();

  /// Join all threads
  std::for_each(threads_.begin(), threads_.end(),
      [](std::thread &t)
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
std::unique_ptr<Error> Master::handle(Session & session)
{
  std::cout << "Master handle (tid=" << std::this_thread::get_id() << ")" << std::endl;

  // Create and get the Packet object from the session (buff_ & length_)
  Packet packet = session.get_packet();

  const std::string& buffer = packet.message_get();

  {
    std::ofstream f1("recieved.txt", std::ios::app);
    f1.write(buffer.c_str(), packet.size_get());
  }

  if (packet.size_get() < 1)
    return std::make_unique<Error>(Error::ErrorType::failure);

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
  return std::make_unique<Error>(Error::ErrorType::failure);
}
