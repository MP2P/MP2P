#include <iostream>
#include <algorithm>
#include <libtools.hh>
#include <iterator>
#include <algorithm>

namespace network
{
  Master::Master(std::unique_ptr<libconfig::Config>&& config)
    : config_{std::move(config)},
      threads_{}
  {
    port_ = utils::get_port(config_);
    concurent_threads_ = utils::get_concurent_threads(config_);
    std::cout << "Concurency level = " << concurent_threads_
              << std::endl << "Bind port = " << port_ << std::endl;

    server_ = std::make_unique<Server>(io_service_, port_, MASTER);
  }

  Master::~Master()
  {
    if (!threads_.empty())
      stop();
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
              std::cout << "Thread " << i + 1 << " j= " << j << " launched!" << std::endl;
              io_service_.run();
            }));
      }
    }
  }

  /// Causes the server to stop it's running threads if any.
  void Master::stop()
  {
    /// Send a stop signal for all threads
    // TODO

    /// Join all threads
    std::for_each(threads_.begin(), threads_.end(),
                  [](std::thread& t) { t.join(); });

    // Delete all threads
    while (!threads_.empty())
      threads_.pop_front();
  }
}
