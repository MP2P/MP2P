#define CATCH_CONFIG_MAIN

#include "../catch.hh"

#include "../../libtools/utils/shared-buffer.hh"
#include "../../libtools/files/files.hh"
#include "../../libtools/network/network.hh"
#include <fstream>
#include <cstdio>
#include <memory>
#include <vector>
#include <fcntl.h>
#include <cstring>
#include <ctime>

using copy = utils::shared_buffer::copy;

TEST_CASE("Shared-buffer", "[libtools][shared-buffer]")
{
  using namespace utils;
  SECTION("Construction")
  {
    SECTION("Size")
    {
      REQUIRE_NOTHROW(utils::shared_buffer{0});
      REQUIRE_NOTHROW(utils::shared_buffer{9});
    }

    SECTION("Move container")
    {
      std::vector<char> vector;
      REQUIRE_NOTHROW(utils::shared_buffer{std::move(vector)});
    }

    SECTION("POD with copy")
    {
      char* text = const_cast<char*>("MP2P");
      size_t size = 4;
      shared_buffer::copy to_copy = copy::Yes;

      utils::shared_buffer buffer(text, size, to_copy);
      REQUIRE(buffer.size() == size);

      for (size_t i = 0; i < size; ++i)
      {
        if (text[i] != buffer.data()[i])
          FAIL("Buffer not equal");
      }
    }

    SECTION("POD without copy")
    {
      const char* text = "MP2P";
      size_t size = 4;
      shared_buffer::copy to_copy = copy::No;

      utils::shared_buffer buffer(text, size, to_copy);
      REQUIRE(buffer.data() == text);
      REQUIRE(buffer.size() == size);
    }
  }

  SECTION("Size")
  {
    utils::shared_buffer number{42};
    REQUIRE(number.size() == 42);

    auto vector_ptr =
      std::make_shared<utils::shared_buffer::container_type>(42, 'A');
    utils::shared_buffer shared{42};
    REQUIRE(shared.size() == 42);

    std::vector<char> vector{42, 'A'};
    utils::shared_buffer moved{42};
    REQUIRE(moved.size() == 42);
  }

  SECTION("String")
  {
    const char* text = "MP2P";
    size_t size = 4;
    shared_buffer::copy to_copy = copy::Yes;

    utils::shared_buffer buffer(text, size, to_copy);
    REQUIRE(buffer.string_get() == "MP2P");
  }
}

TEST_CASE("Files", "[libtools][files]")
{
  using namespace files;

  // Prepare a file
  std::string filename{"MP2P.test"};
  std::string text{"MP2P\n"};
  int fd = open(filename.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0777);
  write(fd, text.c_str(), text.size());
  close(fd);

  SECTION("Construction")
  {
    REQUIRE_THROWS(File{""});

    // FIXME : It becomes a problem when we don't have any file to test
    REQUIRE_NOTHROW(File{filename});
  }

  // Init
  File file{filename};

  SECTION("Filename")
  {
    REQUIRE(filename == file.filepath_get());
  }

  SECTION("Size")
  {
    REQUIRE(file.size() > 0);
    REQUIRE(file.size() == text.size());
  }

  SECTION("Hash")
  {
    auto hash = hash_file(file);
    REQUIRE(hash.size() == 40);
    REQUIRE(hash == "cadf513acf5102b344aa0a840805642b13bc71c3");
  }

  SECTION("Data")
  {
    REQUIRE(file.data() != nullptr);
    REQUIRE(strncmp(file.data(), text.c_str(), text.size()) == 0);
  }

  SECTION("Empty_file")
  {
    std::string empty_fname{"empty"};
    size_t size = 32;
    File empty = File::create_empty_file(empty_fname, size);
    REQUIRE(empty.size() == size);
    REQUIRE(empty.filepath_get() == empty_fname);
    std::remove(empty_fname.c_str()); // Always clean  up
    // FIXME : test hash?
  }

  SECTION("Hash")
  {
    std::string hash = hash_buffer(text.c_str(), text.size());
    REQUIRE(hash == "cadf513acf5102b344aa0a840805642b13bc71c3");
    std::string file_hash = hash_buffer(file.data(), text.size());
    REQUIRE(file_hash == "cadf513acf5102b344aa0a840805642b13bc71c3");
  }

  SECTION("Parts")
  {
    // FIXME : Add tests for part calculations
  }

  std::remove(filename.c_str());

  // Clean up
}

TEST_CASE("Packet", "[libtools][packet]")
{
  using namespace network;
  using namespace network::masks;

  fromto_type fromto = 0;
  what_type what = 0;

  SECTION("Construction")
  {
    const CharT* ptr = nullptr;
    size_type size = 0;
    REQUIRE_NOTHROW((Packet{size, fromto, what, ptr}));

    const CharT* str = "MP2P\n";
    REQUIRE_NOTHROW((Packet{5, fromto, what, str}));

    auto vector = std::vector<masks::CharT>(5, 'O');
    REQUIRE_NOTHROW((Packet{fromto, what, std::move(vector)}));

    SECTION("No message")
    {
      PACKET_HEADER header{5, {fromto, what}};
      REQUIRE_NOTHROW((Packet{header}));
      REQUIRE_NOTHROW((Packet{fromto, what}));
    }
  }

  SECTION("SIZE")
  {
    SECTION("From string")
    {
      std::string str{"MP2P\n"};
      Packet p{static_cast<size_type>(str.size()), fromto, what, str.c_str()};
      REQUIRE(str.size() == p.size_get());
    }

    SECTION("Empty")
    {
      Packet p{fromto, what};
      PACKET_HEADER header{0, {fromto, what}};
      Packet p1{header};

      REQUIRE(p.size_get() == 0);
      REQUIRE(p1.size_get() == 0);
    }

    SECTION("Empty + add_message")
    {
      std::string s{"MP2P\n"};
      Packet p{fromto, what};
      p.add_message(s.c_str(), s.size(), copy::No);
      REQUIRE(p.size_get() == s.size());
      p.add_message(message_type{s.c_str(), s.size(), copy::No});
      REQUIRE(p.size_get() == s.size() + s.size());
    }

    SECTION("Empty + copy_message")
    {
      std::string s{"MP2P\n"};
      message_type message{s.c_str(), s.size(), copy::No};
      REQUIRE(message.size() == s.size());
      Packet p{fromto, what};
      p.copy_message(message);
      REQUIRE(p.size_get() == s.size());
    }

    SECTION("Serialize header")
    {
      std::string s{"MP2P\n"};
      Packet p{fromto, what, message_type{s.c_str(), s.size(), copy::No}};

      PACKET_HEADER header{5, {fromto, what}};
      REQUIRE(std::memcmp(reinterpret_cast<const void*>(&header),
                     reinterpret_cast<const void*>(p.serialize_header().data()),
                     sizeof (PACKET_HEADER)) == 0);

      PACKET_HEADER header_big{999, {fromto, what}};
      Packet p_big{header_big};
      REQUIRE(std::memcmp(reinterpret_cast<const void*>(&header_big),
                     reinterpret_cast<const void*>(p_big.serialize_header().data()),
                     sizeof (PACKET_HEADER)) == 0);
    }
  }
}

TEST_CASE("Network tools", "[libtools][network]")
{
  SECTION("get ipv6")
  {
    auto ipv6 = network::get_ipv6("0.0.0.0");
    REQUIRE(std::string("::ffff:0.0.0.0") == ipv6.to_string());
    ipv6 = network::get_ipv6("0:0:0:0:0:0:0:1");
    REQUIRE(std::string("::1") == ipv6.to_string());
    ipv6 = network::get_ipv6("::1");
    REQUIRE(std::string("::1") == ipv6.to_string());
    ipv6 = network::get_ipv6("localhost");
    REQUIRE(std::string("::1") == ipv6.to_string());

    auto endpoint = network::endpoint_from_host("0.0.0.0");
    REQUIRE(std::string("0.0.0.0") == endpoint.address().to_string());
    endpoint = network::endpoint_from_host("0:0:0:0:0:0:0:1", 3738);
    REQUIRE(std::string("::1") == endpoint.address().to_string());
    endpoint = network::endpoint_from_host("::1");
    REQUIRE(std::string("::1") == endpoint.address().to_string());
    endpoint = network::endpoint_from_host("localhost");
    REQUIRE(std::string("::1") == endpoint.address().to_string());
  }
}
