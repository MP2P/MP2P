#include <network.hh>
#include <masks/messages.hh>

namespace network
{

  Packet::Packet(size_type size,
                 fromto_type fromto,
                 what_type what,
                 message_type message)
    : header_{size, {fromto, what} },
      message_{message}
  {
  }

  Packet::Packet(const PACKET_HEADER& header)
    : header_(header),
      message_{header.size}
  {
  }

  Packet::Packet(size_type size,
                 fromto_type fromto,
                 what_type what,
                 const char* data)
      : header_{size, {fromto, what} },
        message_{data, size}
    {
    }


  Packet::Packet(fromto_type fromto, what_type what, const char* message,
                 std::string hash, size_t partid, size_type size)
    : header_{size, {fromto, what} },
      message_{ 0 }

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
    auto* data = boost::asio::buffer_cast<const unsigned char*>(message_.buffer_get());
    std::memcpy(&*res.begin() + sizeof(header_), data, header_.size);
    return message_type(ptr);
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
