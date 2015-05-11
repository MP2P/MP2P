#include <ostream>

#include <utils.hh>
#include "storage.hh"

int main()
{
  using namespace storage;
  try
  {
    utils::init(); // Throws if anything goes bad

    Storage storage{};

    if (storage.run())
      storage.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Storage failed : " + std::string(e.what());
  }
}
