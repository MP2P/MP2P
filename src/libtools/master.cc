#include <iostream>
#include <algorithm>
#include <utility>
#include <libtools.hh>
#include <iterator>
#include <algorithm>

namespace network
{
  Master::Master(std::unique_ptr<libconfig::Config>&& config)
    : config_{std::move(config)},
      port_{utils::get_port(config_)},
      server_{io_service_, port_,
              std::bind(&Master::handle, this, std::placeholders::_1)}
  {
    concurent_threads_ = utils::get_concurent_threads(config_);
    std::ostringstream msg;
    msg << "Concurency level = " << concurent_threads_;
    utils::print(std::cout, w_mutex_, msg.str());
    msg.str("");
    msg << "Bind port = " << port_;
    utils::print(std::cout, w_mutex_, msg.str());
  }

  Master::~Master()
  {
    if (!threads_.empty())
      stop();
  }

  // Handle the session after filling the buffer
  KeepAlive Master::handle(Session& session)
  {
    std::ostringstream msg;
    msg << "Master handle called. (Thread " << std::this_thread::get_id() << ")";
    utils::print(std::cout, w_mutex_, msg.str());
    msg.str("");

    auto& buff = session.buff_get();
    auto length = session.length_get();

    // Convert the buffer to a string
    std::string line;
    boost::asio::streambuf::const_buffers_type bufs = buff.data();
    line = std::string(boost::asio::buffers_begin(bufs),
                     boost::asio::buffers_begin(bufs) + length);
    buff.consume(length); // Empty the buffer

    auto packet = Packet::deserialize(line); // Get a Packet from the string
    msg << packet;
    utils::print(std::cout, w_mutex_, msg.str());

    // For testing purposes, just send "SEND" through the client to test sending
    if (packet.message_get() == "SEND")
    {
      std::string message("TESTING SENDING");
      Packet p{1, 2, message};
      session.send(p);
      return KeepAlive::Live;
    }

    // FIXME : Close me maybe
    return KeepAlive::Live; // Always keep the connection alive
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
            std::ostringstream msg;
            msg << "Thread " << i + 1 << " launched (id=" << std::this_thread::get_id() << ")!";
            utils::print(std::cout, w_mutex_, msg.str());
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
}
