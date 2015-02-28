#include <iostream>
#include <algorithm>
#include <utility>
#include <libtools.hh>

namespace network
{
  Master::Master(std::unique_ptr<libconfig::Config>&& config)
    : config_{std::move(config)}, port_{utils::get_port(config_)},
      server_{io_service_, port_, std::bind(&Master::handle, this)}
  {
    port_ = utils::get_port(config_);
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

  void Master::handle()
  {
    //std::ostringstream msg;
    //msg << "Connection accepted. (Thread " << std::this_thread::get_id()
              //<< ")" << std::endl;
    utils::print(std::cout, w_mutex_, "Master handle called");
    auto& buff = server_.buff_get();
    auto& socket = server_.socket_get();
    // Read until we see a newline
    boost::asio::async_read_until(socket,
                                  buff,
                                  '\n',
        [this, &buff](boost::system::error_code ec, std::size_t length)
        {
          std::string line;
          if (!ec)
          {
            boost::asio::streambuf::const_buffers_type bufs = buff.data();
            line = std::string(boost::asio::buffers_begin(bufs),
                             boost::asio::buffers_begin(bufs) + length);
            std::cout << line << std::endl;
            buff.consume(length);
          }
          // If the message is wait : then don't close the socket
          if (line == std::string("wait\r\n"))
          {
            handle();
          }
          else // Close the socket and listen for more
          {
            server_.socket_get().close();
            server_.listen();
          }
        });
  }

  /// Creates threads & make them bind the same port defined in the config.
  void Master::run()
  {
    /// Creating (concurent_threads) threads
    for (unsigned i = 0; i < concurent_threads_; ++i)
      threads_.emplace_front(std::thread(
            [i, this]()
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
