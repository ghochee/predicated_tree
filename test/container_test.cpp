#include "tree/tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace std;

TEST_CASE("constructors", "[constructors]") {
    SECTION("empty") {
        tree<int>();
    }
}
