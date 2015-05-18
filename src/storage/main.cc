#include <ostream>

#include <utils.hh>
#include "storage.hh"

int main()
{
  using namespace storage;
  try
  {
    std::string config_path("../config/server.conf");
    // Throws if anything goes bad
    utils::init(config_path);

    Storage storage{};

    if (storage.run())
      storage.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Storage failed : " + std::string(e.what());
  }
}
