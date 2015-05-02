#include <network.hh>
#include <masks/messages.hh>

namespace network
{


  Packet::Packet(const PACKET_HEADER& header)
    : header_(header),
      message_{utils::shared_buffer{header.size}.buffer_get()}
  {
  }

  Packet::Packet(size_type size,
                 fromto_type fromto,
                 what_type what,
                 const char* data)
    : header_{size, {fromto, what} },
      message_{message_type{(void*)data, size}}
  {
  }

  Packet::Packet(size_type size,
                 fromto_type fromto,
                 what_type what,
                 const std::shared_ptr<std::vector<char>>& data)
    : header_{size, {fromto, what} },
      message_{message_type{&*data->begin(), size}}
  {
  }


  Packet::Packet(fromto_type fromto, what_type what, const char* message,
                 std::string hash, size_t partid, size_type size)
    : header_{size, {fromto, what} },
      message_{message_type{(void*)message, size}}

  {
    // FIXME : use sizeof int for the partid
    std::stringstream s;
    s << partid << "|" << hash << std::string(message, size);
    header_.size = boost::asio::buffer_size(message_);
  }

  // Create a std::string from the Packet
  const message_type Packet::serialize() const
  {
    auto ptr = std::make_shared<std::vector<char>>(sizeof(header_)
                                                          + header_.size);
    auto& res = *ptr;
    std::memcpy(&*res.begin(), &header_, sizeof(header_));
    auto* data = boost::asio::buffer_cast<const unsigned char*>(message_[0]);
    std::memcpy(&*res.begin() + sizeof(header_), data, header_.size);
    return message_type(&*ptr->begin(), header_.size);
  }

  // Get a packet from a string
  Packet deserialize(const PACKET_HEADER header, const message_type& message)
  {
    return Packet(boost::asio::buffer_size(message),
                  header.type.fromto,
                  header.type.what,
                  message);
  }

  std::ostream &operator<<(std::ostream &output, const Packet &packet)
  {
    output << string_from(packet.size_get()) << "|"
    << string_from(packet.fromto_get()) << "|"
    << string_from(packet.what_get());
    return output;
  }
}
