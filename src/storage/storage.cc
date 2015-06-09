#include <iostream>

#include <utils.hh>
#include "storage.hh"
#include <masks/messages.hh>

namespace storage
{
  using namespace network;
  using namespace network::masks;
  using namespace boost::asio;
  using namespace boost::posix_time;

  using copy = utils::shared_buffer::copy;

  Storage::Storage()
      : server_{get_ipv6(storage::conf.hostname), storage::conf.port,
                io_service_,
                std::bind(&Storage::recv_dispatcher, this,
                          std::placeholders::_1, std::placeholders::_2),
                std::bind(&Storage::send_dispatcher, this,
                          std::placeholders::_1, std::placeholders::_2)}
  {
    utils::Logger::cout() << "Concurency level = "
                          + std::to_string(storage::conf.concurrency);
    utils::Logger::cout() << "Bind port = "
                          + std::to_string(storage::conf.port);
  }
  uint32_t Storage::id = 0;

  Storage::~Storage()
  {
    if (!threads_.empty())
      stop();
  }

  // Creates threads & make them bind the same port defined in the config.
  bool
  Storage::run()
  {
    init_id();

    if (!server_.is_running())
    {
      stop();
      return false;
    }

    utils::Logger::cout() << "Launching storage with id : "
                             + std::to_string(Storage::id);

    for (unsigned i = 0; i < storage::conf.concurrency; ++i)
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
  void
  Storage::stop()
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
  }

  void
  Storage::init_id()
  {
    std::ifstream id_file(storage::conf.id_path);
    if (!id_file)
    {

      auto master_session = Session{io_service_,
                                    conf.master_hostname,
                                    conf.master_port};

      // Send a request for an id
      s_m::id_req req{storage::conf.port};
      Packet to_send{s_m::fromto, s_m::id_req_w};
      to_send.add_message(&req, sizeof (s_m::id_req), copy::No);
      master_session.send(to_send);

      master_session.blocking_receive(
          [this](Packet p, Session&)
          {
            const CharT* data = p.message_seq_get().front().data();
            const auto* response = reinterpret_cast<const m_s::fid_info*>(data);
            id = response->stid;

            std::ofstream id_file(storage::conf.id_path);
            id_file << response->stid;

            io_service_.stop();

            return 1;
          }
      );

      // The io_service is ran after the IO operations have been "commited"
      io_service_.run();

    }
    else
      id_file >> id;
  }

  // When CTRL+C is typed, we call storage::stop();
  void
  Storage::catch_stop()
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
  masks::ack_type
  Storage::recv_dispatcher(Packet packet, Session& session)
  {
    if (packet.size_get() < 1)
      return 1;

    switch (packet.fromto_get())
    {
      case c_s::fromto:
        switch (packet.what_get())
        {
          case c_s::up_act_w:
            return cs_up_act(packet, session);
          case c_s::down_act_w:
            return cs_down_act(packet, session);
          default:
            return 1;
        }
      default:
        return 100; // Error
    }
  }

  masks::ack_type
  Storage::send_dispatcher(Packet packet, Session& session)
  {
    (void)packet;
    (void)session;
    return 0;
  }
}
