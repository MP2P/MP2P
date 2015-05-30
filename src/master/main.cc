#include <ostream>

#include <utils.hh>
#include "master.hh"


int main(int argc, const char *argv[])
{
  using namespace master; // Use the namespace in the main function only

  master::conf config;
  try
  {
    parse_options(argc, argv, config);
  }
  catch (int i)
  {
    return i;
  }

  try
  {
    std::string config_path("../config/server.conf");
    // Throws if anything goes bad
    utils::init(config_path);
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
