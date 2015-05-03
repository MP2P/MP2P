namespace network
{
  namespace tools
  {
    inline auto resolve_host(std::string& host)
    {
      boost::asio::io_service io_service;
      boost::asio::ip::tcp::resolver resolver(io_service);
      boost::asio::ip::tcp::resolver::query query(host, "");
      return resolver.resolve(query);
    }

    // Returns the first ip it resolved.
    inline auto one_ip_from_host(std::string& host)
    {
      for(boost::asio::ip::tcp::resolver::iterator i = resolve_host(host);
          i != boost::asio::ip::tcp::resolver::iterator();
          ++i)
      {
        boost::asio::ip::tcp::endpoint end = *i;
        return end.address();
      }
      throw std::logic_error("Could not resolve " + host + " ip(s).");
    }
  }
}