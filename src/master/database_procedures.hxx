namespace DB
{
  namespace tools
  {
    // FIXME: cache this for x minutes
    inline
    uint32_t
    number_of_parts(fsize_type file_size)
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

      DB::Connector::get_instance().cmd_put("files", "{"
      "\"count\":" + std::to_string(count + 1) + ","
      "\"total_size:" + std::to_string(total_size + file_size) + "}");

      DB::Connector::get_instance().cmd_put("file." + fi.name_get(),
                                            fi.serialize());
      return fi;
    }

    inline
    std::vector<STPFIELD>
    get_stpfields_for_upload(FileItem fi)
    {
      std::vector<STPFIELD> fields;
      for (stid_type i = 0; i < nb_parts; ++i)
      {
        STPFIELD field = { get_addr("0:0:0:0:0:0:0:1", 3728), 1 };
        fields.push_back(field);
      }
    }
  }
}
