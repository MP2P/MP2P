#include <network.hh>

namespace network
{

  Packet::Packet(uint8_t fromto,
      uint8_t what,
      std::string message)
      : fromto_(fromto),
        what_(what),
        message_(message)
  {
    size_ = message.length();
  }

  Packet::Packet(uint8_t fromto,
      uint8_t what,
      const char* message,
      size_t size)
      : size_(size),
        fromto_(fromto),
        what_(what),
        message_{message, size}
  {
  }

  Packet::~Packet()
  {
  }

  // FIXME : create a hxx and inline them
  uint32_t Packet::size_get() const
  {
    return size_;
  }

  uint8_t Packet::fromto_get() const
  {
    return fromto_;
  }

  uint8_t Packet::what_get() const
  {
    return what_;
  }

  const std::string &Packet::message_get() const
  {
    return message_;
  }

  // Create a std::string from the Packet
  // FIXME : Remove useless chars
  const std::string Packet::serialize() const
  {
    std::string s(6 + message_.size(), '\0');

    char* ptr = &*s.begin();
    uint32_t* sizep = reinterpret_cast<uint32_t*>(ptr);
    *sizep = size_;
    uint8_t* fromtop = reinterpret_cast<uint8_t*>(ptr + sizeof (size_));
    *fromtop = fromto_;
    uint8_t* whatp = reinterpret_cast<uint8_t*>(ptr + sizeof (size_) + sizeof(fromto_));
    *whatp = what_;

    ptr += 6;
    memcpy(ptr, message_.c_str(), message_.size());

    return s;
  }

  // Get a packet from a string
  const Packet Packet::deserialize(const std::string &input)
  {
    uint32_t size = 0;
    uint8_t fromto = 0;
    uint8_t what = 0;
    std::string message;

    const char* ptr = &*input.begin();

    size = *reinterpret_cast<const uint32_t*>(ptr);
    fromto = *reinterpret_cast<const uint8_t*>(ptr + 4);
    what = *reinterpret_cast<const uint8_t*>(ptr + 5);
    message.resize(size, '\0');
    memcpy(&*message.begin(), ptr + 6, size);

    /* FIXME :: Error checks.

    if {
      std::cout << "Invalid packet (" << e.what() << ")" << std::endl;
      return Packet{0, 0, ""};
    }
    unsigned long real_size = (message.length()) * sizeof(uint8_t);
    if (size != real_size)
    {
      std::cout << "Received an invalid packet of size " << real_size
          << " (expecting " << size << ")" << std::endl;
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
