#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <libtools.hh>


using boost::asio::ip::tcp;


namespace network
{
  std::string make_daytime_string()
  {
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
  }

  class tcp_connection
    : public boost::enable_shared_from_this<tcp_connection>
  {
    public:
      typedef boost::shared_ptr<tcp_connection> pointer;

      static pointer create(boost::asio::io_service& io_service)
      {
        return pointer(new tcp_connection(io_service));
      }

      tcp::socket& socket()
      {
        return socket_;
      }
      void start()
      {
        message_ = make_daytime_string();
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
            boost::bind(&tcp_connection::handle_write, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
      }

    private:
      tcp_connection(boost::asio::io_service& io_service)
        : socket_(io_service)
      {
      }

      void handle_write(const boost::system::error_code& /*error*/,
          size_t /*bytes_transferred*/)
      {
        std::cout << "handle_write" << std::endl;
      }

      tcp::socket socket_;
      std::string message_;
  };



  class tcp_server
  {
    public:
      tcp_server(boost::asio::io_service& io_service)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), 13))
      {
        start_accept();
      }
    private:
      void start_accept()
      {
        tcp_connection::pointer new_connection =
          tcp_connection::create(acceptor_.get_io_service());

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&tcp_server::handle_accept, this, new_connection,
              boost::asio::placeholders::error));
      }
      void handle_accept(tcp_connection::pointer new_connection,
          const boost::system::error_code& error)
      {
        if (!error)
        {
          new_connection->start();
        }

        start_accept();
      }
      tcp::acceptor acceptor_;
  };

  /**** CLASS PACKET ****/
  Packet::Packet(uint32_t size, uint8_t fromto, uint8_t what, std::string message)
    : size_(size),
      fromto_(fromto),
      what_(what),
      message_(message)
    {
      std::ostringstream o;
      o << "Packet(" 
        << size_ << ", " << int(fromto_) 
        << ", " << int(what_) << ", " << message_
        << ")";
      utils::print_debug(o.str());
    }
  Packet::~Packet()
    {}
  

  /**** END CLASS PACKET ****/


  void send_message()
  {
    try
    {
      
      auto p_1 = std::make_shared<Packet>(5, 1, 2, "lolilol");
      boost::asio::io_service io_service;
      tcp_server server(io_service);
      io_service.run();
      std::cout << "Sent a message!" << std::endl;
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }
}
