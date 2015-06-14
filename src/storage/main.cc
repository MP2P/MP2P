#include <ostream>

#include <utils.hh>
#include "storage.hh"

int main(int argc, const char* argv[])
{
  using namespace storage;

  try
  {
    parse_options(argc, argv);
    utils::check_system();
  }
  catch (int i)
  {
    return i;
  }
  catch (std::exception& e)
  {
    std::cerr << "Initialization failed " << e.what() << std::endl;
    return 1;
  }

  try
  {
    // Throws if anything goes bad
    Storage storage{};

    if (storage.run())
      storage.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Storage exception: " + std::string(e.what());
    return 1;
  }

  return 0;
}
