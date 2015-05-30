namespace DB
{
  namespace tools
  {
    inline uint32_t number_of_parts(fsize_type file_size)
    {
      std::string json = Connector::get_instance().cmd_get("storage_sizes");

      boost::property_tree::ptree pt;
      std::istringstream is(json);
      boost::property_tree::read_json(is, pt);

      std::vector<std::pair<fsize_type, partnum_type>> values;

      boost::property_tree::ptree values_pt = pt.get_child("values");

      for (auto it = values_pt.begin(); it != values_pt.end(); ++it)
      {
        const boost::property_tree::ptree &pt2 = it->second;
        values.push_back(std::pair<fsize_type, partnum_type>(
            pt2.get<fsize_type>("file_size"),
            pt2.get<partnum_type>("nb_part")
        ));
      }

      for (auto it = values.begin() ; it != values.end(); ++it)
      {
        if (file_size <= it->first)
          return it->second;
      }
      return 0;
    }
  }
}