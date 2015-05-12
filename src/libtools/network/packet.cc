#include <network.hh>

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

  Packet::Packet(fromto_type fromto,
                 what_type what,
                 const std::shared_ptr<std::vector<CharT>>& data)
    : header_{static_cast<size_type>(data->size()), {fromto, what} },
      message_seq_{message_type{data}}
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
    add_message(message_type{message.data(), message.size(), copy::Yes});
  }

  const message_type Packet::serialize_header() const
  {
    const char* p_header = reinterpret_cast<const char*>(&header_);
    return message_type(p_header, sizeof(header_), copy::Yes);
  }

  Packet deserialize(const PACKET_HEADER header, const message_type& message)
  {
    return Packet(header.type.fromto,
                  header.type.what,
                  message);
  }

  std::ostream &operator<<(std::ostream &output, const Packet &p)
  {
    output << "{s: " << utils::misc::string_from(p.size_get())
    << ", f: " << (int)p.fromto_get()
    << ", w: " << (int)p.what_get()
    << "}";
    return output;
  }
}
