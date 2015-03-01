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
      server_{io_service_, port_, std::bind(&Master::handle, this, std::placeholders::_1)}
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
    // FIXME : Use some kind of stringstream
    // utils::print(std::cout, w_mutex_, "Master handle called");
    std::cout << "Master handle called. (Thread " << std::this_thread::get_id()
              << ")" << std::endl;
    auto& buff = session.buff_get();
    auto length = session.length_get();
    //
    // Read until we see a newline
    std::string line;
    boost::asio::streambuf::const_buffers_type bufs = buff.data();
    line = std::string(boost::asio::buffers_begin(bufs),
                     boost::asio::buffers_begin(bufs) + length);
    //std::cout << line;
    buff.consume(length);

    auto packet = Packet::deserialize(line);
    std::cout << packet;

    // For testing purposes, just send "SEND" through the client to test sending
    if (packet.message_get() == "SEND")
    {
      std::string message("TESTING SENDING");
      Packet p{1, 2, message};
      session.send(p);
      return KeepAlive::Live;
    }

    return KeepAlive::Live;
  }

  /// Creates threads & make them bind the same port defined in the config.
  void Master::run()
  {
    /// Creating (concurent_threads) threads
    for (unsigned i = 0; i < concurent_threads_; ++i)
    {
      unsigned j = i;
      if (i == 0)
        threads_.emplace_front(std::thread(
              [i, this]()
              {
                std::cout << "Thread " << i + 1 << " launched!" << std::endl;
              }));
      else
      {
        auto it = threads_.begin();
        std::advance(it, i - 1);

        threads_.insert_after(it, std::thread(
            [&i, j, this]()
            {
              try
              {
                std::ostringstream msg;
                msg << "Thread " << i + 1 << " launched!";
                utils::print(std::cout, w_mutex_, msg.str());
                io_service_.run();
              }
              catch (std::exception& e)
              {
                std::cerr << "Fail: " << e.what() << std::endl;
              }
            })
        );
      }
    }
  }

  /// Causes the server to stop it's running threads if any.
  void Master::stop()
  {
    /// Send a stop signal for all threads
    // TODO

    /// Join all threads
    std::for_each(threads_.begin(), threads_.end(), [](std::thread& t){ t.join(); });

    // Delete all threads
    while (!threads_.empty())
      threads_.pop_front();
  }
}
