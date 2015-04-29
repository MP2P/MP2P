#include <network.hh>
#include <masks/messages.hh>

namespace network
{

  Packet::Packet(size_type size,
                 fromto_type fromto,
                 what_type what,
                 message_type message)
      : message_(message)
  {
    header_.size = size;
    header_.type = { fromto, what };
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
    message_ = boost::asio::const_buffer(&*s.str().begin(), s.str().size());
    header_.size = boost::asio::buffer_size(message_);
  }

  // Create a std::string from the Packet
  const message_type Packet::serialize() const
  {
    std::vector<unsigned char> res(sizeof(header_) + header_.size);
    std::memcpy(&*res.begin(), &header_, sizeof(header_));
    auto* data = boost::asio::buffer_cast<const unsigned char*>(message_);
    std::memcpy(&*res.begin() + sizeof(header_), data, header_.size);
    return message_type(&*res.begin(), res.size());
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
