#define CATCH_CONFIG_MAIN

#include <network.hh>
#include "../../master/database.hh"
#include "../catch.hh"

TEST_CASE("Items can be serialized & deserialized", "[db-items]" ) {

  network::PARTID partid = {1, 3};
  std::string hash = "azertyuiopmlkjhgfdsq";
  std::vector<network::stid_type> vect = {3, 4, 5, 6};

  // Defining new items
  Database::PartItem pi = Database::PartItem(partid, hash, vect);
  std::string spi = pi.serialize();

  Database::FileItem fi =
      Database::FileItem(1, "filename.txt", 15000, 3, 1, hash, true);
  std::string sfi = fi.serialize();

  REQUIRE(fi.id_get() == 1);
  REQUIRE(fi.name_get() == "filename.txt");
  REQUIRE(fi.file_size_get() == 15000);
  REQUIRE(fi.redundancy_get() == 3);
  REQUIRE(fi.current_redundancy_get() == 1);
  std::string res_has(fi.hash_get());
  REQUIRE(res_has.compare(hash));
  REQUIRE(fi.is_uploaded());
  REQUIRE_FALSE(fi.is_replicated()); // 3 == 1

}