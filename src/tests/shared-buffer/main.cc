#define CATCH_CONFIG_MAIN

#include "../catch.hh"
#include "../../libtools/utils/shared-buffer.hh"

TEST_CASE("Constructors", "[constructions]")
{
  utils::shared_buffer(0);
}
