#include <ostream>

#include <utils.hh>
#include "master.hh"
#include <fcntl.h>


int main(int argc, const char *argv[])
{
  using namespace master; // Use the namespace in the main function only

  try
  {
    parse_options(argc, argv);
    utils::check_system();

    // Initialize DB connexion
    DB::Connector::get_instance(master::conf.db.hostname,
                                master::conf.db.password,
                                master::conf.db.bucket);
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
  catch (...)
  {
    std::cerr << "Fatal error: could not initialize." << std::endl;
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
    return 1;
  }
  catch (...)
  {
    utils::Logger::cerr() << "Master: Fatal error.";
    return 1;
  }
  return 0;
};
