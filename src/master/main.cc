#include <ostream>

#include <utils.hh>
#include "master.hh"
#include "database.hh"

int main()
{
  try
  {
    // Throws if anything goes bad
    utils::init();

    const std::string host = utils::Conf::get_instance().DBhost_get();
    const std::string password = utils::Conf::get_instance().DBpassword_get();
    const std::string bucket = utils::Conf::get_instance().DBbucket_get();

    // Throws if anything goes bad
    static Database::CouchbaseDb db = Database::CouchbaseDb(host, password,
                                                            bucket);
    utils::Logger::cout() << "Successfully connected to database.";

    Master master;
    if (master.run())
      master.catch_stop();
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
    std::exit(EXIT_FAILURE);
  }
}