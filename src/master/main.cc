#include <ostream>

#include <utils.hh>
#include "master.hh"


int main()
{
  try
  {
    // Throws if anything goes bad
    utils::init();
    Database::Connector::get_instance(); // Initialize connexion
  }
  catch (std::exception& e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
    return 1;
  }

  // Test
  std::cout << Database::tools::number_of_parts(2199023255551) << std::endl;

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