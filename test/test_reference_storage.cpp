#include "allocator/reference_storage.hpp"
#include "allocator/memory_pool.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


TEST_CASE("reference_storage", "[reference_storage]")
{
    SECTION("ctor and copy ctor")
    {
        reference_storage rs1;
        reference_storage rs2(rs1);
        reference_storage rs3;
        rs3 = rs2;
    }

    SECTION("is_valid")
    {
        reference_storage rs;
        REQUIRE_FALSE(rs.is_valid());
    }

    SECTION("value ctor")
    {
        memory_pool mp(16, 1024);
        reference_storage rs(mp);
        REQUIRE(rs.is_valid());

        memory_pool& mp2 = rs.get_allocator();
        REQUIRE(&mp == &mp2);
    }
}
