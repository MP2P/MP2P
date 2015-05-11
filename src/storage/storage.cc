#include <iostream>

#include <utils.hh>
#include "storage.hh"

namespace storage
{
  using namespace network;
  using namespace network::masks;
  using namespace boost::asio;
  using namespace boost::posix_time;

  Storage::Storage()
      : server_{io_service_,
                std::bind(&Storage::dispatcher, this, std::placeholders::_1,
                          std::placeholders::_2)}
  {
    unsigned concurrency = utils::Conf::get_instance().concurrency_get();
    unsigned port = utils::Conf::get_instance().port_get();

    utils::Logger::cout() << "Concurency level = " + std::to_string(concurrency);
    utils::Logger::cout() << "Bind port = " + std::to_string(port);
  }

  Storage::~Storage()
  {
    if (!threads_.empty())
      stop();
  }

  // Creates threads & make them bind the same port defined in the config.
  bool Storage::run()
  {
    if (!server_.is_running())
    {
      stop();
      return false;
    }
    // Creating (concurent_threads) threads
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
  void Storage::stop()
  {
    utils::Logger::cout() << "The server is going to stop...";
    server_.stop();

    // Join all threads
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

    // Delete all threads
    while (!threads_.empty())
      threads_.pop_front();
  }

  // When CTRL+C is typed, we call storage::stop();
  void Storage::catch_stop()
  {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = [](int s)
    {
      utils::Logger::cout() << "Storage received signal " + std::to_string(s)
                               + "...";
    };
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    pause();

    stop();

    utils::Logger::cout() << "Storage: Bye bye!";
  }

  // Handle the session after filling the buffer
  // Errors are defined in the ressources/errors file.
  error_code Storage::dispatcher(Packet packet, Session& session)
  {
    (void) session;
    std::ostringstream s;
    s << std::this_thread::get_id();
    utils::Logger::cout() << "Storage dispatcher (tid=" + s.str() + ").";

    // Create and get the Packet object from the session (buff_ & length_)
    if (packet.size_get() < 1)
      return 1;

    auto& buf = packet.message_seq_get()[0];

    std::istringstream input;
    input.rdbuf()->pubsetbuf(buffer_cast<CharT*>(buf), buffer_size(buf));

    std::string item;

    std::getline(input, item, '|');
    std::string part(item);
    char hash_c[41] = { 0 };
    input.read(hash_c, 40);
    std::string hash(hash_c);
    std::getline(input, item, '|');
    std::string msg(item);

    {
      std::ofstream f1(part);
      f1.write(&*msg.begin(), msg.size());
    }

    switch (packet.fromto_get())
    {
      // FIXME : dispatcher storage
      default:
        return 100; // Error
    }
  }
}
