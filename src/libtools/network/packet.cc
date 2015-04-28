#include <network.hh>
#include <masks/messages.hh>

namespace network
{

  Packet::Packet(fromto_type fromto, what_type what, message_type message)
      : message_(message)
  {
    header_.type.fromto = fromto;
    header_.type.what = what;
    if (message.size() > std::numeric_limits<size_type>::max())
      throw std::logic_error("Message size is too large to fit in a "
                             + std::string(typeid(size_type).name()));
    header_.size = (size_type) message.size();
  }

//  Packet::Packet(fromto_type fromto, what_type what,
//                 const char* message, size_t size)
//      : size_(size),
//        fromto_(fromto),
//        what_(what),
//        message_{message, size}
//  {
//  }
//
  Packet::Packet(fromto_type fromto, what_type what, const char* message,
                 std::string hash, size_t partid, size_t size)
  {
    header_.size = size;
    header_.type = { fromto, what };
    // FIXME : use sizeof int for the partid
    std::stringstream s;
    s << partid << "|" << hash << std::string(message, size);
    message_ = std::vector<unsigned char>(s.str().begin(), s.str().end());
    header_.size = message_.size();
  }

  // Create a std::string from the Packet
  const message_type Packet::serialize() const
  {
    std::vector<unsigned char> res(sizeof(header_) + header_.size);
    std::memcpy(&*res.begin(), &header_, sizeof(header_));
    std::memcpy(&*res.begin() + sizeof(header_), message_.data(), header_.size);
    return res;
  }

  // Get a packet from a string
  Packet deserialize(const PACKET_HEADER header, const message_type& message)
  {
    return Packet(header.type.fromto, header.type.what, message);
  }

  std::ostream &operator<<(std::ostream &output, const Packet &packet)
  {
    output << string_from(packet.size_get()) << "|"
    << string_from(packet.fromto_get()) << "|"
    << string_from(packet.what_get());
    return output;
  }
}
