#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <libtools.hh>


using boost::asio::ip::tcp;


namespace network
{

  Packet::Packet(unsigned fromto,
                 unsigned what,
                 std::string message)
    : fromto_(fromto),
      what_(what),
      message_(message)
    {
      size_ = sizeof(fromto) + sizeof(what) + message.size();
    }
  Packet::~Packet()
    {}

  size_t Packet::size_get()
  {
    return size_;
  }

  unsigned Packet::fromto_get()
  {
    return fromto_;
  }

  unsigned Packet::what_get()
  {
    return what_;
  }

  std::string& Packet::message_get()
  {
    return message_;
  }

  // Create a std::string from the Packet
  // FIXME : Remove useless chars
  const std::string Packet::serialize() const
  {
    std::ostringstream packet;
    packet << size_ << "|"
           << fromto_ << "|"
           << what_ << "|"
           << message_
           << "\n";
    return packet.str();
  }

  // Get a packet from a string
  const Packet Packet::deserialize(const std::string& input)
  {
    size_t size = 0;
    unsigned fromto = 0;
    unsigned what = 0;
    std::string message;

    std::istringstream packet(input);
    std::string item;
    std::getline(packet, item, '|');
    size = std::stoi(item);
    std::getline(packet, item, '|');
    fromto = std::stoi(item);
    std::getline(packet, item, '|');
    what = std::stoi(item);
    std::getline(packet, message, '\n');
    if (size != (sizeof(fromto) + sizeof(what) + message.size()))
      throw std::runtime_error("Error while deserializing Packet");
    return Packet{fromto, what, message};
  }

  std::ostream &operator<<(std::ostream& output, const Packet& packet)
  {
    output << packet.serialize();
    return output;
  }

}
