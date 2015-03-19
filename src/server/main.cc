#include <utils.hh>
#include "master.hh"

#include <ostream>

int main()
{
  if (!utils::init())
    return 1;

  try
  {
    Master master{};

    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Server failed : " << e.what() << std::endl;
  }
}
