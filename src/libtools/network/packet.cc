#include <network.hh>
#include <masks/messages.hh>

namespace network
{
  using namespace boost::asio;
  using namespace utils;
  using namespace network::masks;

  using copy = utils::shared_buffer::copy;

  Packet::Packet(size_type size,
                 fromto_type fromto,
                 what_type what,
                 const CharT* data)
    : header_{size, {fromto, what}},
      message_seq_{message_type{data, size, copy::Yes}}
  {
  }

  Packet::Packet(masks::size_type size,
                 masks::fromto_type fromto,
                 masks::what_type what,
                 const masks::CharT* data,
                 copy to_copy)
    : header_{size, {fromto, what}},
      message_seq_{message_type{data, size, to_copy}}
  {
  }


  Packet::Packet(masks::fromto_type fromto,
                 masks::what_type what,
                 const utils::shared_buffer& message)
    : header_{static_cast<size_type>(message.size()), {fromto, what}},
      message_seq_{message}
  {
  }

  Packet::Packet(fromto_type fromto,
                 what_type what,
                 std::vector<CharT>&& data)
    : header_{static_cast<size_type>(data.size()), {fromto, what} },
      message_seq_{message_type{std::move(data)}}
  {
  }

  Packet::Packet(const PACKET_HEADER& header)
    : header_(header)
  {
  }

  Packet::Packet(fromto_type fromto,
                 what_type what)
    : header_{0, {fromto, what}}
  {
  }

  void Packet::copy_message(const message_type& message)
  {
    add_message(message.data(), message.size(), copy::Yes);
  }

  const message_type Packet::serialize_header() const
  {
    const char* p_header = reinterpret_cast<const char*>(&header_);
    return message_type(p_header, sizeof(header_), copy::Yes);
  }

  std::ostream &operator<<(std::ostream &output, const Packet &p)
  {
    output << "{s: " << utils::misc::string_from(p.size_get())
    << ", f: " << (int)p.fromto_get()
    << ", w: " << (int)p.what_get()
    << "}";
    return output;
  }

  masks::fromto_type fromto_inverse(masks::fromto_type fromto)
  {
    switch (fromto)
    {
      case masks::c_m::fromto:
        return m_c::fromto;
      case m_c::fromto:
        return c_m::fromto;
      case c_s::fromto:
        return s_c::fromto;
      case s_c::fromto:
        return c_s::fromto;
      case s_m::fromto:
        return m_s::fromto;
      case m_s::fromto:
        return s_m::fromto;
      default:
        assert(!"Invalid fromto");
    }
  }
}
