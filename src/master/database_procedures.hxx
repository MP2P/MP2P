namespace DB
{
  namespace tools
  {
    // For get_stpfields_for_upload
    static bool iterator_initialized = false;
    static std::vector<StorageItem>::iterator current_round_robin;
    static std::vector<StorageItem> storages{};


    // FIXME: cache this for x minutes
    inline
    network::masks::partnum_type
    number_of_parts(fsize_type file_size) // See ressources/data/storage_sizes.json
    {
      std::string json = Connector::get_instance().cmd_get("storage_sizes");

      boost::property_tree::ptree pt;
      std::istringstream is(json);
      boost::property_tree::read_json(is, pt);

      std::vector<std::pair<fsize_type, partnum_type>> values;

      try
      {
        boost::property_tree::ptree values_pt = pt.get_child("values");

        for (auto it = values_pt.begin(); it != values_pt.end(); ++it)
        {
          const boost::property_tree::ptree& pt2 = it->second;
          values.push_back(std::pair<fsize_type, partnum_type>(
              pt2.get<fsize_type>("file_size"),
              pt2.get<partnum_type>("nb_part")
          ));
        }
      }
      catch (boost::property_tree::ptree_error)
      {
        throw std::runtime_error("Error: Database returned results with bad "
                                 "type, or with overfloaded values.");
      }

      for (auto it = values.begin() ; it != values.end(); ++it)
      {
        if (file_size <= it->first)
          return it->second;
      }
      return 0;
    }

    // FIXME: cache this for x minutes
    inline
    std::vector<StorageItem>
    get_all_storages()
    {
      std::string json = DB::Connector::get_instance().cmd_get("storages");
      DB::MetaOnStoragesItem mos = DB::MetaOnStoragesItem::deserialize(json);

      std::vector<StorageItem> storages;
      for (stid_type i = 0; i < mos.count_get(); ++i)
      {
        json = DB::Connector::get_instance().cmd_get("st." + std::to_string(i));
        storages.push_back(StorageItem::deserialize(json));
      }

      iterator_initialized = false;

      return storages;
    }

    // FIXME: CAS this
    inline
    void
    create_new_file(DB::FileItem& fi)
    {
      // Check if filename already exists
      uint64_t i = 0;

      std::string tmp_name = fi.name_get();
      bool good_name = false;
      do
      {
        try
        {
          DB::Connector::get_instance().cmd_get("file."
                                                + tmp_name
                                                + ".__" + std::to_string(i));
          ++i;
        }
        catch (...)
        {
          fi.name_set(tmp_name + ".__" + std::to_string(i));
          good_name = true;
        }
      } while (!good_name);

      std::string json = DB::Connector::get_instance().cmd_get("files");
      DB::MetaOnFilesItem mof = DB::MetaOnFilesItem::deserialize(json);

      // Update fi id
      fi.id_set(mof.count_get());

      // Update count & total_size of metadata on files
      mof.count_set(mof.count_get() + 1);
      mof.total_size_set(mof.total_size_get() + fi.file_size_get());

      // Add file in name_by_id list
      mof.name_by_id_get().emplace(fi.id_get(), fi.name_get());

      // Update metadata on files
      DB::Connector::get_instance().cmd_put("files", mof.serialize());

      // Put the FileItem in DB
      DB::Connector::get_instance().cmd_put("file." + fi.name_get(), fi.serialize());
    }

    inline
    std::vector<STPFIELD>
    get_stpfields_for_upload(fsize_type file_size)
    {
      if (!iterator_initialized)
      {
        auto tmp = get_all_storages();

        if (tmp.size() == 0)
          throw std::logic_error("There is no storage in DB. Add at least one.");

        storages.assign(tmp.begin(), tmp.end());
        std::srand(std::time(0));
        stid_type x = std::rand() % (storages.size() + 1);
        current_round_robin = std::next(storages.begin(), x);
        iterator_initialized = true;
      }

      // FIXME: check if available space on storages is OK
//      uint64_t total_available_space = 0;
//      for (auto it : storages)
//      {
//        total_available_space += it.available_space_get();
//        std::cout << "Storage : " << it.serialize() << std::endl;
//      }
//
//      if (total_available_space < file_size)
//      {
//        throw std::logic_error("Storages are full.");
//      }

      // Round-robin distribution over all Storages.

      // <Storage, count>
      std::vector<std::pair<StorageItem, partnum_type>> tmp_values;

      auto nb_parts = number_of_parts(file_size);

      for (network::masks::partnum_type i = 1; i <= nb_parts; ++i)
      {
        if (current_round_robin == storages.end())
          current_round_robin = storages.begin();

        auto part_size = network::get_part_size(file_size,
                                                i - 1,
                                                nb_parts);

        // If this storage does not have enough place, check the next one.
        if (current_round_robin->available_space_get() < part_size)
          --i;
        else
        {
          auto f = tmp_values.begin();
          for (; f < tmp_values.end(); ++f)
          {
            if (f->first.host_addr_get() == current_round_robin->host_addr_get()
                && f->first.port_get() == current_round_robin->port_get())
            {
              f->second += 1;
              break;
            }
          }
          // Else add the storage to fields with nb=1 if it's ipv6 it joinable
          if (f == tmp_values.end())
          {
            try
            {
              current_round_robin->addr_get();
              tmp_values.push_back({*current_round_robin, 1});
            }
            catch (...)
            {
              --i;
            }
          }
        }
        ++current_round_robin;
      }

      std::vector<STPFIELD> fields;
      for (auto it : tmp_values)
        fields.push_back({it.first.addr_get(), it.second});
      return fields;
    }
  }
}
