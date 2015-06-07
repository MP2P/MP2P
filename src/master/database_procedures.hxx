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

    // FIXME: cache this for x minutes(size_t size, size_t part_id, size_t parts)
    inline
    std::vector<StorageItem>
    get_all_storages()
    {
      std::string json = DB::Connector::get_instance().cmd_get("storages");

      boost::property_tree::ptree pt;
      std::istringstream is(json);
      boost::property_tree::read_json(is, pt);

      std::vector<StorageItem> storages;
      stid_type count = pt.get<stid_type>("count");

      for (stid_type i = 0; i < count; ++i)
      {
        json = DB::Connector::get_instance().cmd_get("st." + std::to_string(i));
        storages.push_back(StorageItem::deserialize(json));
      }

      iterator_initialized = false;

      return storages;
    }

    // FIXME: CAS this
    inline
    FileItem
    create_new_file(std::string name, fsize_type file_size,
                    rdcy_type redundancy, std::string hash)
    {
      std::string json = DB::Connector::get_instance().cmd_get("files");

      boost::property_tree::ptree pt;
      std::istringstream is(json);
      boost::property_tree::read_json(is, pt);

      uint64_t count = pt.get<uint64_t>("count");
      uint64_t total_size = pt.get<uint64_t>("total_size");

      auto fi = FileItem(count + 1, name, file_size, redundancy, 0, hash, 0);

      DB::Connector::get_instance().cmd_put("files", ("{"
      "\"count\":" + std::to_string(count + 1) + ","
      "\"total_size:" + std::to_string(total_size + file_size) + "}"));

      DB::Connector::get_instance().cmd_put("file." + fi.name_get(),
                                            fi.serialize());
      return fi;
    }

    inline
    std::vector<STPFIELD>
    get_stpfields_for_upload(FileItem fi)
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
        std::cout << "current_round_robin_storage=" << std::to_string(x) << " (count=" << std::to_string(storages.size()) << ")" << std::endl;
        iterator_initialized = true;
      }

      uint64_t total_available_space = 0;
      for (auto it : storages)
      {
        total_available_space += it.available_space_get();
      }

      if (total_available_space < fi.file_size_get())
      {
        throw std::logic_error("Storages are full.");
      }

      // Round-robin distribution over all Storages.
      std::vector<STPFIELD> fields;
      auto nb_parts = number_of_parts(fi.file_size_get());
      for (network::masks::partnum_type i = 1; i <= nb_parts; ++i)
      {
        if (current_round_robin == storages.end())
          current_round_robin = storages.begin();
        else
          ++current_round_robin;

        auto part_size = network::get_part_size(fi.file_size_get(), i - 1, nb_parts);

        // If this storage does not have enough place, check the next one.
        if (current_round_robin->available_space_get() < part_size)
          --i;
        else
        {
          ADDR tmp_addr = current_round_robin->addr_get();

          // Check if the storage isn't already in our fields list,
          // If so, just add 1 to f.nb.
          auto f = fields.begin();
          for (; f < fields.end(); ++f)
          {
            if (f->addr.ipv6 == tmp_addr.ipv6 && f->addr.port == tmp_addr.port)
            {
              f->nb += 1;
            }
          }
          // Else add the storage to fields with nb=1.
          if (f == fields.end())
            fields.push_back({current_round_robin->addr_get(), 1});
        }
      }
      return fields;
    }
  }
}
