#define CATCH_CONFIG_MAIN

#include <network.hh>
#include "../../master/master.hh"
#include "../catch.hh"

using namespace network::masks;


TEST_CASE("Items can be serialized & deserialized", "[db-items]")
{
  // Initializing requirements
  std::string hash = "azertyuiopmlkjhgfdsq";
  PARTID partid = {1, 3};
  std::vector <stid_type> vect = {3, 4, 5, 6};
  std::string host_addr = "master.mp2p.mydomain.com";
  std::string res_has;


  SECTION("Create, serialize, deserialize a PartItem")
  {
    DB::PartItem part_item = DB::PartItem(partid, hash, vect);
    std::string s_part_item = part_item.serialize();
    part_item = DB::PartItem::deserialize(s_part_item);

    // Check the resulting PartItem
    REQUIRE(part_item.fid_get() == partid.fid);
    REQUIRE(part_item.num_get() == partid.partnum);
    res_has = std::string(reinterpret_cast<const char*>(part_item.hash_get()));
    REQUIRE(res_has.compare(hash));
    std::vector <stid_type> v = part_item.locations_get();
    REQUIRE(std::accumulate(v.begin(), v.end(), 0) == (3 + 4 + 5 + 6));
  }


  SECTION("Create, serialize, deserialize a FileItem")
  {
    DB::PartItem part_item = DB::PartItem(partid, hash, vect);

    DB::FileItem file_item(1, "filename.txt", 15000, 3, 1, hash, true, {part_item});
    std::string s_file_item = file_item.serialize();
    file_item = DB::FileItem::deserialize(s_file_item);
    DB::PartItem res_part_item = *file_item.parts_get().begin();

    // Check the resulting FileItem
    REQUIRE(file_item.id_get() == 1ULL);
    REQUIRE(file_item.name_get() == "filename.txt");
    REQUIRE(file_item.file_size_get() == 15000ULL);
    REQUIRE(file_item.redundancy_get() == 3);
    REQUIRE(file_item.current_redundancy_get() == 1);
    res_has = std::string(reinterpret_cast<const char*>(file_item.hash_get()));
    REQUIRE(res_has.compare(hash));
    REQUIRE(file_item.is_uploaded());
    REQUIRE(res_part_item.fid_get() == partid.fid);
    REQUIRE(res_part_item.num_get() == partid.partnum);

    REQUIRE_FALSE(file_item.is_replicated()); // 3 == 1
  }


  SECTION("Create, serialize, deserialize a MasterItem")
  {
    DB::MasterItem master_item = DB::MasterItem(42, host_addr);
    std::string s_master_item = master_item.serialize();
    master_item = DB::MasterItem::deserialize(s_master_item);

    // Check the resulting MasterItem
    REQUIRE(master_item.id_get() == 42);
    REQUIRE(master_item.host_addr_get() == host_addr);
  }


  SECTION("Create, serialize, deserialize a StorageItem")
  {
    DB::StorageItem storage_item = DB::StorageItem(4242, host_addr, 3728,
                                                   10000000042);
    std::string s_storage_item = storage_item.serialize();
    storage_item = DB::StorageItem::deserialize(s_storage_item);

    // Check the resulting MasterItem
    REQUIRE(storage_item.id_get() == 4242);
    REQUIRE(storage_item.host_addr_get() == host_addr);
    REQUIRE(storage_item.port_get() == 3728);
    REQUIRE(storage_item.available_space_get() == 10000000042ULL);
  }


  SECTION("Create, serialize, deserialize a MetaOnFilesItem")
  {
    std::unordered_map<fid_type, std::string> tmp;
    for (uint16_t i = 0; i < 62; i += 3)
      tmp.insert({i, "filename" + std::to_string(i)});

    DB::MetaOnFilesItem mof_item = DB::MetaOnFilesItem(62, 125588150, tmp);
    std::string s_mof_item = mof_item.serialize();
    mof_item = DB::MetaOnFilesItem::deserialize(s_mof_item);

    // Check the resulting MasterItem
    REQUIRE(mof_item.count_get() == 62ULL);
    REQUIRE(mof_item.total_size_get() == 125588150);
    REQUIRE(mof_item.file_name_by_id(3) == "filename3");
  }


  SECTION("Create, serialize, deserialize a MetaOnStoragesItem")
  {
    DB::MetaOnStoragesItem mos_item = DB::MetaOnStoragesItem(62, 125588150);
    std::string s_mos_item = mos_item.serialize();
    mos_item = DB::MetaOnStoragesItem::deserialize(s_mos_item);

    // Check the resulting MasterItem
    REQUIRE(mos_item.count_get() == 62);
    REQUIRE(mos_item.available_space_get() == 125588150);
  }
}
