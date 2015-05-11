#define CATCH_CONFIG_MAIN

#include "../../libtools/utils/shared-buffer.hh"

#include <memory>
#include <vector>
#include "../catch.hh"

TEST_CASE("Constructors", "[shared-buffer]")
{
  SECTION("Size")
  {
    REQUIRE_NOTHROW(utils::shared_buffer{0});
    REQUIRE_NOTHROW(utils::shared_buffer{9});
  }

  SECTION("Shared_ptr")
  {
    auto vector_ptr = std::make_shared<utils::shared_buffer::container_type>();
    REQUIRE_NOTHROW(utils::shared_buffer{vector_ptr});
    REQUIRE_NOTHROW(utils::shared_buffer{nullptr});
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
    bool copy = true;

    utils::shared_buffer buffer(text, size, copy);
    REQUIRE(buffer.size() == size);

    for (size_t i = 0; i < size; ++i)
    {
      if (text[i] != buffer.data_get()[i])
        FAIL("Buffer not equal");
    }
  }

  SECTION("POD without copy")
  {
    const char* text = "MP2P";
    size_t size = 4;
    bool copy = false;

    utils::shared_buffer buffer(text, size, copy);
    REQUIRE(utils::buffer_cast<char*>(buffer) == text);
    REQUIRE(buffer.size() == size);
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
    bool copy = true;

    utils::shared_buffer buffer(text, size, copy);
    REQUIRE(buffer.string_get() == "MP2P");
  }
}
