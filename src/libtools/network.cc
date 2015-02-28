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
      std::ostringstream o;
      o << "Packet("
        << size_ << ", " << int(fromto_)
        << ", " << int(what_) << ", " << message_
        << ")";
      utils::print_debug(o.str());
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

  const std::string Packet::serialize() const
  {
    std::ostringstream packet;
    packet << size_ << "|"
           << fromto_ << "|"
           << what_ << "|"
           << message_;
    return packet.str();
  }

}
