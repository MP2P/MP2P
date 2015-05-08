namespace database
{
  namespace tools
  {
    inline uint32_t number_of_parts(std::string& json, fsize_type file_size)
    {
      boost::property_tree::ptree pt;
      std::istringstream is(json);
      boost::property_tree::read_json(is, pt);

      std::vector<std::pair<fsize_type, partnum_type>> values;

      BOOST_FOREACH(const boost::property_tree::ptree::value_type &v,
                    pt.get_child("values"))
      {
        const boost::property_tree::ptree &pt2 = v.second;
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