#include <memory>
#include <unistring/stdint.h>
#include "master.hh"

// FROMTO = 0
std::unique_ptr<Error> Handle_CM(Packet & packet, Session & session)
{
  if (packet.get_size() && session.length_get())
  {
  }
  if (packet.get_what()
  return std::make_unique<Error>(Error::ErrorType::success);
}

std::unique_ptr<Error> Handle_SM(Packet & packet, Session & session)
{
  if (packet.get_size() && session.length_get())
  {
  }
  return std::make_unique<Error>(Error::ErrorType::success);
}

uint16_t Handle_MM(Packet & packet, Session & session)
{
  if (packet.get_size() && session.length_get())
  {
  }
  return std::make_unique<Error>(Error::ErrorType::success);
}
