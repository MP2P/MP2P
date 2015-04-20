#include <ostream>

#include <utils.hh>
#include "master.hh"
#include "DbConnector.hh"

int main()
{
  try
  {
    utils::init(); // Throws if anything goes bad

    Master master;

    if (!DbConnector::get_instance().Initialize())
      throw std::logic_error("Could not connect to database.");
    else
      utils::Logger::cout() << "Successfully connected to database.";

    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Master failed : " + std::string(e.what());
    return 1;
  }
}