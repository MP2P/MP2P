#include <ostream>
#include <utils.hh>
#include "master.hh"
#include "database.hh"

static Database::Database* db = nullptr;

int main()
{
  /* TODO: put in test folder with catch framework */
//  network::PARTID partid = {1, 3};
//  std::string hash = "azertyuiopmlkjhgfdsq";
//  std::vector<network::stid_type> vect = {3, 4, 5, 6};
//
//  Database::PartItem pi = Database::PartItem(partid, hash, vect);
//  std::string spi = pi.serialize();
//
//  std::cout << spi << std::endl;
//
//  Database::FileItem fi = Database::FileItem(1, "filename.txt", 15000, 3, 1,
//                                             hash, true);
//
//  std::string sfi = fi.serialize();
//  std::cout << sfi<< std::endl;
//
//  Database::PartItem dfi = Database::PartItem::deserialize(spi);
//  std::cout << utils::misc::separate(dfi.locations_get(), "|") << std::endl;

  /* ENDTODO */


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