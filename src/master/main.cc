#include <ostream>
#include <utils.hh>
#include "master.hh"
#include "database.hh"

static Database::Database* db = nullptr;

int main()
{
  std::string host;
  std::string password;
  std::string bucket;
  try
  {
    // Throws if anything goes bad
    utils::init();

    host = utils::Conf::get_instance().DBhost_get();
    password = utils::Conf::get_instance().DBpassword_get();
    bucket = utils::Conf::get_instance().DBbucket_get();
  }
  catch (std::exception& e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
  }

  try
  {
    // Throws if anything goes bad
    db = new Database::CouchbaseDb(host, password, bucket);
    utils::Logger::cout() << "Successfully connected to database.";

    Master master;
    if (master.run())
      master.catch_stop();
  }
  catch (Couchbase::Status& s)
  {
    utils::Logger::cerr() << "Master exception: Invalid database configuration"
                            "(couchbase://" + host + "/" + bucket + ").";
  }
  catch (std::exception &e)
  {
    utils::Logger::cerr() << "Master exception: " + std::string(e.what());
  }

  utils::Logger::cout() << "Exiting...";
  if (db != nullptr)
    delete db;

  return 0;
};