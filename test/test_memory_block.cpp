#include "allocator/memory_block.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::size_t
#include <cstdint>
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
        REQUIRE(mb.contains(static_cast<std::uint8_t*>(mb.memory) + 10));
        std::free(mb.memory);
    }

    SECTION("constexpr")
    {
        constexpr memory_block mb1;
        REQUIRE(mb1.memory == nullptr);
        REQUIRE(mb1.size == 0);
        REQUIRE_FALSE(mb1.contains(nullptr));

        constexpr memory_block mb2(nullptr, sizeof(int));
        REQUIRE(mb2.memory == nullptr);
        REQUIRE(mb2.size == 4);
        REQUIRE_FALSE(mb2.contains(nullptr));
    }
}
