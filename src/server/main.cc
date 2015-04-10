#include <utils.hh>
#include "master.hh"

#include <ostream>

int main()
{
  try
  {
    utils::init(); // Throws if anything goes bad

    Master master{};

    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    std::cerr << "Server failed : " << e.what() << std::endl;
  }
}
