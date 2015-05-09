#include <ostream>

#include <utils.hh>
#include "master.hh"


int main()
{
  try
  {
    // Throws if anything goes bad
    utils::init();
    DB::Connector::get_instance(); // Initialize connexion
  }
  catch (std::exception& e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
    return 1;
  }

  try
  {
    // Throws if anything goes bad
    Master master{};
    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
  }

  utils::Logger::cout() << "Exiting...";
  return 0;
};