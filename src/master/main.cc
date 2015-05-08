#include <ostream>

#include <utils.hh>
#include "master.hh"


int main()
{
  std::string host;
  std::string pass;
  std::string bucket;
  try
  {
    // Throws if anything goes bad
    utils::init();

    host = utils::Conf::get_instance().DBhost_get();
    pass = utils::Conf::get_instance().DBpassword_get();
    bucket = utils::Conf::get_instance().DBbucket_get();
  }
  catch (std::exception& e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
    return 1;
  }

  try
  {
    // Throws if anything goes bad
    Master master(host, pass, bucket);
    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
  }

  utils::Logger::cout() << "Exiting...";

//  if (db_instance != nullptr)
//    delete db_instance;

  return 0;
};