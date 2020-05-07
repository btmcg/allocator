#include "allocator/memory_arena.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


TEST_CASE("memory_block", "[memory_block]")
{
    SECTION("ctor")
    {
        memory_block mb;
        REQUIRE(mb.memory == nullptr);
        REQUIRE(mb.size == 0);
        REQUIRE_FALSE(mb.contains(nullptr));
    }

    SECTION("from malloc")
    {
        constexpr int size = 128;
        memory_block mb(std::malloc(size), size);
        REQUIRE(mb.memory != nullptr);
        REQUIRE(mb.size == size);
        REQUIRE_FALSE(mb.contains(nullptr));
        REQUIRE(mb.contains(static_cast<std::byte*>(mb.memory) + 10));
        std::free(mb.memory);
    }
}
