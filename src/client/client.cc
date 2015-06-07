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

  namespace // Anonymous namespace - used for local helpers
  {
    size_t part_size_for_sending_size(size_t size, size_t part_id, size_t parts)
    {
      // FIXME : float may not fit in size_t
      size_t part_size = std::ceil((float)size / parts);
      if (part_id == (parts - 1))
      {
        size_t offset = part_id * part_size;
        if ((offset + part_size) > size)
          part_size -= offset + part_size - size;
      }
      return part_size;
    }
  }

  Client::Client(const std::string& host, uint16_t port)
    : master_session_{io_service_, host, port,
        std::bind(&Client::recv_handle, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&Client::send_handle, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&Client::remove_handle, this, std::placeholders::_1)}
  {
  } 
  void
  Client::remove_handle(Session& session)
  {
    (void) session;
  }

  error_code
  Client::recv_handle(Packet packet, Session& session)
  {
    (void) session;
    (void) packet;

    return 0;
  }

  error_code
  Client::send_handle(Packet packet, Session& session)
  {
    (void) session;
    (void) packet;

    Logger::cout() << "Packet sent!";

    return 0;
  }

  void
  Client::run()
  {
    io_service_.run();
  }

  void
  Client::stop()
  {
    // FIXME : Stop everything, join threads if needed
  }

  void Client::request_upload(const files::File& file,
                              rdcy_type rdcy)
  {
    fsize_type fsize = file.size();
    const std::string& fname = file.filename_get();

    c_m::up_req request{fsize, rdcy, {}}; // The request message

    Packet req_packet{0, 1};
    req_packet.add_message(reinterpret_cast<CharT*>(&request),
                           sizeof (request),
                           copy::No);

    // FIXME : fname.size() may not fit in uint32_t
    req_packet.add_message(fname.c_str(), fname.size(), copy::No);
    master_session_.send(req_packet);

    master_session_.blocking_receive(
        [&file, this](Packet p, Session& /*recv_session*/) -> error_code
        {
          CharT* data = p.message_seq_get().front().data();
          m_c::up_pieces_loc* pieces = reinterpret_cast<m_c::up_pieces_loc*>(data);

          // Get the number of STPFIELDS
          size_t list_size = (p.size_get() - sizeof (fid_type))
                              / sizeof (STPFIELD);

          // Get total number of parts
          size_t total_parts = 0;
          for (size_t i = 0; i < list_size; ++i)
            total_parts += pieces->fdetails.stplist[i].nb;

          size_t parts = total_parts;

          for (size_t i = 0; i < list_size; ++i)
          {
            STPFIELD& field = pieces->fdetails.stplist[i];
            send_parts(pieces->fdetails.fid,
                       file, field.addr,
                       total_parts,
                       parts - field.nb, parts);
            parts -= field.nb;
          }

          return 0;
        });
  }

  void Client::send_parts(fid_type fid,
                          const files::File& file,
                          const ADDR& addr,
                          size_t total_parts,
                          size_t begin_id, size_t end_id)
  {
    // Create an unique thread per session
    std::thread sending{
      [&file, this, &addr, total_parts, begin_id, end_id, fid]() {
        auto host = network::binary_to_string_ipv6(addr.ipv6,
                                               network::masks::ipv6_type_size);
        // Create the storage session
        Session storage{io_service_, host, addr.port};

        // For each part to send, create a Packet and send it synchronously
        for (size_t i = begin_id; i < end_id; ++i)
        {
          // Get the exact part size depending on the part id
          // This allows us to avoid the last part to be bigger
          size_t part_size = part_size_for_sending_size(file.size(), i,
                                                        total_parts);

          const auto* part_buffer = file.data()
                                    + (file.size() / total_parts) * i;
          partnum_type part_num = i;

          auto hash = files::hash_buffer_hex(part_buffer, part_size);

          Packet to_send{c_s::fromto, c_s::up_act_w};
          to_send.add_message(reinterpret_cast<const CharT*>(&fid),
                              sizeof (fid), copy::Yes);
          to_send.add_message(reinterpret_cast<const CharT*>(&part_num),
                              sizeof (part_num), copy::Yes);

          to_send.add_message(reinterpret_cast<const CharT*>(hash.data()),
                              hash.size(), copy::Yes);
          // FIXME : part_size may not fit in uint32_t
          to_send.add_message(part_buffer, part_size, copy::No);
          storage.send(to_send);
        }
      }
    };

    // FIXME : URGENT. Add to a vector to be joined at the end
    sending.join();
  }

  // Send a c_m::down_req to the master
  void Client::request_download(const std::string& filename)
  {
    // Prepare the request packet: contains only the filename.
    // There is no need for a custom structure
    Packet request{c_m::fromto, c_m::down_req_w};
    request.add_message(filename.c_str(), filename.size(), copy::Yes);

    master_session_.send(request);

    // Wait for an answer from the master, then connect to each of the
    // storages
    master_session_.blocking_receive(
        [&filename, this](Packet p, Session& /*recv_session*/) -> error_code
        {
          CharT* data = p.message_seq_get().front().data();
          m_c::down_pieces_loc* pieces = reinterpret_cast<m_c::down_pieces_loc*>(data);

          // Get the number of STPFIELDS
          size_t list_size = (p.size_get()
                              - sizeof (fid_type) - sizeof (fsize_type))
                             / sizeof (STPFIELD);

          // Get the size of a part
          auto part_size = pieces->fsize / list_size;

          // Create an empty file, resized to the size of the expected file
          auto file = files::File::create_empty_file(filename + "-dl",
                                                     pieces->fsize);

          for (size_t i = 0; i < list_size; ++i)
          {
            // Recieve a part directly into the file
            STPFIELD& field = pieces->fdetails.stplist[i];
            recv_part(file,
                      field.addr,
                      part_size);
          }

          return 0;
        });
  }

  void Client::recv_part(files::File& file,
                         const ADDR& addr,
                         size_t part_size)
  {
    std::thread recv{
      [&file, this, &addr, part_size]() {
        auto host = network::binary_to_string_ipv6(addr.ipv6,
                                               network::masks::ipv6_type_size);
        // Create the storage session
        Session storage{io_service_, host, addr.port};

        // Receive a part
        storage.blocking_receive(
            [&file, part_size](Packet p, Session&) -> error_code
            {
              CharT* data = p.message_seq_get().front().data();
              s_c::up_act* upload = reinterpret_cast<s_c::up_act*>(data);

              // Write the data to the file
              memcpy(file.data() + upload->partid.partnum * part_size,
                     upload->data,
                     p.size_get() - sizeof (PARTID) - sizeof (sha1_type));
              return 0;
            }
        );
      }
    };

    // FIXME : URGENT. Add to a vector to be joined at the end
    recv.join();
  }
}
