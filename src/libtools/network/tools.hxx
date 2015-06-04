namespace network
{
  inline
  boost::asio::ip::tcp::resolver::iterator
  resolve_host(const std::string& host, std::string port/* = "" */)
  {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(host, port);
    return resolver.resolve(query);
  }

  inline
  boost::asio::ip::tcp::resolver::iterator
  resolve_host(const std::string& host, uint16_t port)
  {
    std::string str_port = boost::lexical_cast<std::string>(port);
    return resolve_host(host, str_port);
  }

  // Returns the first ip it resolved.
  inline
  boost::asio::ip::address one_ip_from_host(const std::string& host)
  {
    for (auto i = resolve_host(host);
         i != boost::asio::ip::tcp::resolver::iterator();)
    {
      boost::asio::ip::tcp::endpoint end = *i;
      return end.address();
    }
    throw std::logic_error("Could not resolve " + host + " ip(s).");
  }

  // Returns an ip address from a hostname or from an IPVX string.
  inline
  boost::asio::ip::address_v6 get_ipv6(const std::string& str)
  {
    try
    {
      auto ip = boost::asio::ip::address::from_string(str);
      if (ip.is_v4())
        ip = boost::asio::ip::address_v6::v4_mapped(ip.to_v4());
      return ip.to_v6();
    }
    catch (...)
    {
      auto ip = one_ip_from_host(str);
      if (ip.is_v4())
        ip = boost::asio::ip::address_v6::v4_mapped(ip.to_v4());
      return ip.to_v6();
    }
  }
}