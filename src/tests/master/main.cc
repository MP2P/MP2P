#define CATCH_CONFIG_MAIN

#include <network.hh>
#include "../../master/database.hh"
#include "../catch.hh"

using namespace network::masks;


TEST_CASE("Items can be serialized & deserialized", "[db-items]" )
{
  // Initializing requirements
  std::string hash = "azertyuiopmlkjhgfdsq";
  PARTID partid = {1, 3};
  std::vector <stid_type> vect = {3, 4, 5, 6};
  std::string host_addr = "master.mp2p.mydomain.com";
  std::string res_has;

  SECTION("Create, serialize, deserialize a FileItem") {
    Database::FileItem file_item(1, "filename.txt", 15000, 3, 1, hash, true);
    std::string s_file_item = file_item.serialize();
    file_item = Database::FileItem::deserialize(s_file_item);

    // Check the resulting FileItem
    REQUIRE(file_item.id_get() == 1);
    REQUIRE(file_item.name_get() == "filename.txt");
    REQUIRE(file_item.file_size_get() == 15000);
    REQUIRE(file_item.redundancy_get() == 3);
    REQUIRE(file_item.current_redundancy_get() == 1);
    res_has = std::string(file_item.hash_get());
    REQUIRE(res_has.compare(hash));
    REQUIRE(file_item.is_uploaded());
    REQUIRE_FALSE(file_item.is_replicated()); // 3 == 1
  }


  SECTION("Create, serialize, deserialize a PartItem") {
    Database::PartItem part_item = Database::PartItem(partid, hash, vect);
    std::string s_part_item = part_item.serialize();
    part_item = Database::PartItem::deserialize(s_part_item);

    // Check the resulting PartItem
    REQUIRE(part_item.fid_get() == 1);
    REQUIRE(part_item.num_get() == 3);
    res_has = std::string(part_item.hash_get());
    REQUIRE(res_has.compare(hash));
    std::vector <stid_type> v = part_item.locations_get();
    REQUIRE(std::accumulate(v.rbegin(), v.rend(), 0) == (3 + 4 + 5 + 6));
  }


  SECTION("Create, serialize, deserialize a MasterItem") {
    Database::MasterItem master_item = Database::MasterItem(42, host_addr);
    std::string s_master_item = master_item.serialize();
    master_item = Database::MasterItem::deserialize(s_master_item);

    // Check the resulting MasterItem
    REQUIRE(master_item.id_get() == 42);
    REQUIRE(master_item.host_addr_get() == host_addr);
  }


  SECTION("Create, serialize, deserialize a StorageItem") {
    Database::StorageItem storage_item = Database::StorageItem(4242, host_addr,
                                                               10000000042);
    std::string s_storage_item = storage_item.serialize();
    storage_item = Database::StorageItem::deserialize(s_storage_item);

    // Check the resulting MasterItem
    REQUIRE(storage_item.id_get() == 4242);
    REQUIRE(storage_item.host_addr_get() == host_addr);
    REQUIRE(storage_item.available_space_get() == 10000000042);
  }
}