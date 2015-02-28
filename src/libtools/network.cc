#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <libtools.hh>


using boost::asio::ip::tcp;


namespace network
{

  Packet::Packet(size_t size,
                 char fromto,
                 char what,
                 std::string message)
    : size_(size),
      fromto_(fromto),
      what_(what),
      message_(message)
    {
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

  char Packet::fromto_get()
  {
    return fromto_;
  }

  char Packet::what_get()
  {
    return what_;
  }

  std::string& Packet::message_get()
  {
    return message_;
  }

  std::string Packet::serialize()
  {
    std::ostringstream packet;
    packet << size_ << "|"
           << fromto_ << "|"
           << what_ << "|"
           << message_;
    return packet.str();
  }

}
