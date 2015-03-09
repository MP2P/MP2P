#include <memory>
#include "master.hh"

std::unique_ptr<Error> Handle_CM(Packet& packet, Session& session)
{
  if (packet.size_get() && session.length_get())
  {
  }
  return std::make_unique<Error>(Error::ErrorType::success);
}

std::unique_ptr<Error> Handle_SM(Packet& packet, Session& session)
{
  if (packet.size_get() && session.length_get())
  {
  }
  return std::make_unique<Error>(Error::ErrorType::success);
}

std::unique_ptr<Error> Handle_MM(Packet& packet, Session& session)
{
  if (packet.size_get() && session.length_get())
  {
  }
  return std::make_unique<Error>(Error::ErrorType::success);
}
