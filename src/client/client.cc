#include "client.hh"

#include <utils.hh>
#include <files.hh>
#include <masks/messages.hh>

namespace client
{
  using namespace network;
  using namespace network::masks;
  using namespace utils;
  using copy = utils::shared_buffer::copy;

  namespace
  {
    void process_error(const Packet& p, std::string from)
    {
      const CharT* data = p.message_seq_get().front().data();
      const ack* ack_code = reinterpret_cast<const ack*>(data);

      // FIXME : Error message
      std::ostringstream ss;
      ss << "Error received from " << from << " : " << (int)*ack_code;

      throw std::logic_error(ss.str());
    }
  }

  Client::Client(const std::string& host, uint16_t port)
    : master_session_{io_service_, host, port}
  {
    // Run the network service, right away
    io_service_.run();
  }

  void
  Client::end_all_tasks()
  {
    tasks_.clear(); // Force the destruction of all the futures
  }

  // FIXME : types
  void Client::request_upload(const files::File& file, rdcy_type rdcy)
  {
    if (rdcy == 0)
      throw std::logic_error("Redundancy must be >= 1.");

    fsize_type fsize = file.size();
    const std::string& fname = file.filename_get();

    c_m::up_req request{fsize, rdcy, {}}; // The request message

    Packet req_packet{0, 1};
    req_packet.add_message(&request, sizeof (request), copy::No);
    req_packet.add_message(fname.c_str(), fname.size(), copy::No);
    master_session_.blocking_send(req_packet);

    master_session_.blocking_receive(
        [&file, this](Packet p, Session& /*recv_session*/)
        {
          if (p.what_get() == 0)
            process_error(p, "master");

          CharT* data = p.message_seq_get().front().data();
          m_c::up_pieces_loc* pieces = reinterpret_cast<m_c::up_pieces_loc*>(data);

          utils::Logger::cout() << "Received FDETAILS from master.";

          // Get the number of STPFIELDS
          size_t list_size = (p.size_get() - sizeof (fid_type))
                              / sizeof (STPFIELD);

          utils::Logger::cout() << "Parts repartition for upload:";
          for (size_t i = 0; i < list_size; ++i)
            utils::Logger::cout() << "    -> " + network::binary_to_string_ipv6(pieces->fdetails.stplist[i].addr.ipv6, 16) + ":"
                                     + std::to_string(pieces->fdetails.stplist[i].addr.port)
                                     + " count = " + std::to_string(pieces->fdetails.stplist[i].nb);

          // Get total number of parts
          size_t total_parts = 0;
          for (size_t i = 0; i < list_size; ++i)
            total_parts += pieces->fdetails.stplist[i].nb;

          size_t parts = total_parts;

          utils::Logger::cout() << "Upload started...";
          auto begin = std::chrono::steady_clock::now();
          for (size_t i = 0; i < list_size; ++i)
          {
            STPFIELD& field = pieces->fdetails.stplist[i];
            tasks_.emplace_back(std::async(std::launch::async,
                                send_parts(
                                pieces->fdetails.fid,
                                file,
                                field.addr,
                                total_parts,
                                parts - field.nb,
                                parts))
            );
            parts -= field.nb;
          }

          end_all_tasks();

          auto end = std::chrono::steady_clock::now();

          auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

          utils::Logger::cout() << "Upload took "
                                   + boost::lexical_cast<std::string>(duration)
                                   + " milliseconds ("
                                   + boost::lexical_cast<std::string>(file.size() / duration)
                                   + "Kio/s).";

          return keep_alive::No;
        });
  }

  // FIXME: types
  std::function<void()>
  Client::send_parts(fid_type fid,
                     const files::File& file,
                     const ADDR& addr,
                     size_t total_parts,
                     size_t begin_id, size_t end_id)
  {
    return [&file, this, addr, total_parts, begin_id, end_id, fid]()
    {
        auto host = network::binary_to_string_ipv6(addr.ipv6,
                                                   network::masks::ipv6_type_size);
        // Create the storage session
        Session storage{io_service_, host, addr.port};

        // For each part to send, create a Packet and send it synchronously
        for (size_t i = begin_id; i < end_id; ++i)
        {
          // Get the exact part size depending on the part id
          // This allows us to avoid the last part to be bigger
          size_t part_size = network::get_part_size(file.size(), i, total_parts);

          const auto* part_buffer = file.data()
                                    + (file.size() / total_parts) * i;
          partnum_type part_num = i;

          auto hash = files::hash_buffer_hex(part_buffer, part_size);

          Packet to_send{c_s::fromto, c_s::up_act_w};
          to_send.add_message(&fid, sizeof (fid), copy::No);
          to_send.add_message(&part_num, sizeof (part_num), copy::No);

          to_send.add_message(hash.data(), hash.size(), copy::No);

          // FIXME : part_size may not fit in uint32_t
          to_send.add_message(part_buffer, part_size, copy::No);

          utils::Logger::cout() << "Sending part to storage";

          storage.blocking_send(to_send);

          recv_ack(storage);

          utils::Logger::cout() << "Storage received part";
        }
    };
  }

  // Send a c_m::down_req to the master
  void Client::request_download(const std::string& filename)
  {
    // Prepare the request packet: contains only the filename.
    // There is no need for a custom structure
    Packet request{c_m::fromto, c_m::down_req_w};
    request.add_message(filename.c_str(), filename.size(), copy::No);

    utils::Logger::cout() << "Requesting file " + filename + " to master.";
    master_session_.blocking_send(request);

    // Wait for an answer from the master, then download all parts from storages
    master_session_.blocking_receive(
        [&filename, this](Packet p, Session& /*recv_session*/)
        {
          if (p.what_get() == 0)
            process_error(p, "master");

          CharT* data = p.message_seq_get().front().data();
          m_c::down_pieces_loc* pieces = reinterpret_cast<m_c::down_pieces_loc*>(data);

          // Get the number of STPFIELDS
          size_t list_size = (p.size_get()
                              - sizeof (fid_type) - sizeof (fsize_type))
                             / sizeof (STPFIELD);

          std::cout << "list_size = " + std::to_string(list_size) << std::endl;

          if (list_size == 0) // FIXME : Find one way to treat errors the same
            throw std::logic_error("Invalid packet from master");

          // Get the size of a part
          auto part_size = pieces->fsize / list_size;

          if (pieces->fsize > boost::filesystem::space(".").available)
            throw std::logic_error("Not enough space available on filepath.");

          // Create an empty file, resized to the size of the expected file
          auto file = files::File::create_empty_file(filename + "-dl",
                                                     pieces->fsize);

          for (size_t i = 0; i < list_size; ++i)
          {
            // Recieve a part directly into the file
            STPFIELD& field = pieces->fdetails.stplist[i];
            PARTID partid{ pieces->fdetails.fid, field.nb };
            tasks_.emplace_back(std::async(std::launch::async,
                                recv_part(
                                file,
                                field.addr,
                                partid,
                                part_size))
            );
          }

          end_all_tasks();

          return keep_alive::No;
        });
  }

  std::function<void()>
  Client::recv_part(files::File& file,
                    ADDR addr,
                    PARTID partid,
                    size_t part_size)
  {
    return [&file, this, addr, partid, part_size]()
    {
      auto host = network::binary_to_string_ipv6(addr.ipv6,
                                                 network::masks::ipv6_type_size);

      utils::Logger::cout() << "Requesting part_id=" + std::to_string(partid.partnum)
                               + " (file_id=" + std::to_string(partid.fid)
                               + ") to storage " + host
                               + ":" + std::to_string(addr.port) + ".";

      // Create the storage session
      Session storage{io_service_, host, addr.port};

      Packet to_send{c_s::fromto, c_s::down_act_w};
      to_send.add_message(&partid, sizeof (PARTID), copy::No);

      storage.blocking_send(to_send);

      // Receive a part
      storage.blocking_receive(
          [&file, part_size](Packet p, Session&)
          {
            CharT* data = p.message_seq_get().front().data();
            s_c::up_act* upload = reinterpret_cast<s_c::up_act*>(data);

            // Write the data to the file
            memcpy(file.data() + upload->partid.partnum * part_size,
                   upload->data,
                   p.size_get() - sizeof (PARTID) - sizeof (sha1_type));
            return keep_alive::Yes;
          }
      );
    };
  }
}
