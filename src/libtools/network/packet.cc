#include <network.hh>
#include <masks/messages.hh>

namespace network
{

  Packet::Packet(uint8_t fromto, uint8_t what, std::string message)
      : fromto_(fromto),
        what_(what),
        message_(message)
  {
    size_ = message.length();
  }

  Packet::Packet(uint8_t fromto, uint8_t what, const char* message, size_t size)
      : size_(size),
        fromto_(fromto),
        what_(what),
        message_{message, size}
  {
  }

  Packet::Packet(uint8_t fromto, uint8_t what, const char* message,
                 std::string hash, size_t partid, size_t size)
      : fromto_(fromto),
        what_(what)
  {
    // FIXME : use sizeof int for the partid
    std::stringstream s;
    s << partid << "|" << hash << std::string(message, size);
    message_ = s.str();
    size_ = message_.size();
  }

  // Create a std::string from the Packet
  const std::string Packet::serialize() const
  {
    size_t header_size = sizeof (size_) + sizeof (fromto_) + sizeof (what_);
    std::string s(header_size + message_.size(), '\0');

    char* ptr = &*s.begin();
    uint32_t* sizep = reinterpret_cast<uint32_t*>(ptr);
    *sizep = size_;
    uint8_t* fromtop = reinterpret_cast<uint8_t*>(ptr + sizeof (size_));
    *fromtop = fromto_;
    uint8_t* whatp = reinterpret_cast<uint8_t*>(ptr + sizeof (size_) + sizeof(fromto_));
    *whatp = what_;

    memcpy(ptr + header_size, message_.c_str(), message_.size());

    return s;
  }

  // Get a packet from a string
  const Packet deserialize(const std::string &input)
  {
    uint32_t size = 0;
    uint8_t fromto = 0;
    uint8_t what = 0;
    std::string message;

    size_t header_size = sizeof (size) + sizeof (fromto) + sizeof (what);
    const char* ptr = &*input.begin();

    size = *reinterpret_cast<const uint32_t*>(ptr);
    fromto = *reinterpret_cast<const uint8_t*>(ptr + sizeof (size));
    what = *reinterpret_cast<const uint8_t*>(ptr + sizeof (size)
                                                 + sizeof (fromto));
    message.resize(size, '\0');
    memcpy(&*message.begin(), ptr + header_size, size);

    /* FIXME :: Error checks.

    unsigned long real_size = (message.length()) * sizeof(uint8_t);
    if (size != real_size)
    {
      //std::cout << "Received an invalid packet of size " << real_size
          //<< " (expecting " << size << ")" << std::endl;
      utils::Logger::cout() << "Received an invalid packet of size " << std::to_string(real_size)
          << " (expecting " << std::to_string(size) << ").";
      return Packet{0, 0, ""};
    }

    */

    return Packet{fromto, what, message};
  }

  std::ostream &operator<<(std::ostream &output, const Packet &packet)
  {
    output << packet.size_get() << "|"
           << (int)packet.fromto_get() << "|"
           << (int)packet.what_get() << "|"
           << packet.message_get();
    return output;
  }
}
