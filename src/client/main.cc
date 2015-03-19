#include <utils.hh>
#include "client.hh"

#include <iostream>

int main()
{
  if (!utils::init())
    return 1;

  try
  {
    Client client{};

    client.run();
    client.stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Client failed : " << e.what() << std::endl;
  }
}
