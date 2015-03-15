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
    size_ = (2 + message.length()) * sizeof(uint8_t);
  }

  Packet::~Packet()
  {
  }

  uint32_t Packet::size_get()
  {
    return size_;
  }

  uint8_t Packet::fromto_get()
  {
    return fromto_;
  }

  uint8_t Packet::what_get()
  {
    return what_;
  }

  std::string &Packet::message_get()
  {
    return message_;
  }

  // Create a std::string from the Packet
  // FIXME : Remove useless chars
  const std::string Packet::serialize() const
  {
    std::ostringstream packet;
    packet << size_ << "|"
        << int(fromto_) << "|"
        << int(what_) << "|"
        << message_
        << "\r\n";
    return packet.str();
  }

  // Get a packet from a string
  const Packet Packet::deserialize(const std::string &input)
  {
    uint32_t size = 0;
    uint8_t fromto = 0;
    uint8_t what = 0;
    std::string message;

    std::istringstream packet(input);
    std::string item;

    try
    {
      std::getline(packet, item, '|');
      size = (uint32_t) std::stoi(item);
      std::getline(packet, item, '|');
      fromto = (uint8_t) std::stoi(item);
      std::getline(packet, item, '|');
      what = (uint8_t) std::stoi(item);
      std::getline(packet, message, '\n');
    }
    catch (const std::exception &e)
    {
      std::cout << "Invalid packet (" << e.what() << ")" << std::endl;
      return Packet{0, 0, ""};
    }
    unsigned long real_size = (2 + message.length()) * sizeof(uint8_t);
    if (size != real_size)
    {
      std::cout << "Received an invalid packet of size " << real_size
          << " (expecting " << size << ")" << std::endl;
      return Packet{0, 0, ""};
    }
    return Packet{fromto, what, message};
  }

  std::ostream &operator<<(std::ostream &output, const Packet &packet)
  {
    output << packet.serialize();
    return output;
  }
}
