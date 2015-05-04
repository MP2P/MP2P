#include <ostream>
#include <utils.hh>
#include "master.hh"
#include "database.hh"

#include <string.h>

static Database::Database* db = nullptr;

int main()
{
  network::PARTID partid = {1, 3};
  std::cout << partid.fid << " | " << partid.partnum << std::endl;
  std::string hash = "azertyuiopmlkjhgfdsq";
  std::vector<network::stid_type> vect = {3, 4, 5, 6};

  Database::PartItem pi = Database::PartItem(partid, hash, vect);
  std::cout << pi.serialize() << std::endl;

  try
  {
    // Throws if anything goes bad
    utils::init();

    const std::string host = utils::Conf::get_instance().DBhost_get();
    const std::string password = utils::Conf::get_instance().DBpassword_get();
    const std::string bucket = utils::Conf::get_instance().DBbucket_get();

    // Throws if anything goes bad
    db = new Database::CouchbaseDb(host, password, bucket);
    utils::Logger::cout() << "Successfully connected to database.";

    Master master;
    if (master.run())
      master.catch_stop();
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