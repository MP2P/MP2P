#include <ostream>

#include <utils.hh>
#include "master.hh"

int main()
{
  try
  {
    utils::init(); // Throws if anything goes bad

    Master master;

    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Master failed : " + std::string(e.what());
  }
}
