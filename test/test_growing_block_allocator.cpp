#include "allocator/growing_block_allocator.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


TEST_CASE("growing_block_allocator", "[growing_block_allocator]")
{
    SECTION("ctor")
    {
        growing_block_allocator<> gba1(4096);
        REQUIRE(gba1.growth_factor() == Approx(2.0));
        REQUIRE(gba1.next_block_size() == 4096);

        growing_block_allocator<3, 2> gba2(1024);
        REQUIRE(gba2.growth_factor() == Approx(1.5));
        REQUIRE(gba2.next_block_size() == 1024);
    }

    SECTION("alloc-dealloc")
    {
        growing_block_allocator<> gba(1024);
        REQUIRE(gba.growth_factor() == Approx(2.0));

        REQUIRE(gba.next_block_size() == 1024);
        memory_block mb1 = gba.allocate_block();
        REQUIRE(mb1.memory != nullptr);
        REQUIRE(mb1.size == 1024);

        REQUIRE(gba.next_block_size() == 2048);
        memory_block mb2 = gba.allocate_block();
        REQUIRE(mb2.memory != nullptr);
        REQUIRE(mb2.memory != mb1.memory);
        REQUIRE(mb2.size == 2048);

        gba.deallocate_block(mb1);
        gba.deallocate_block(mb2);
    }

    SECTION("constexpr")
    {
        constexpr growing_block_allocator<2, 1> gba(1024);
        constexpr memory_block mb;
        gba.deallocate_block(mb);
        REQUIRE(gba.next_block_size() == 1024);
        REQUIRE(gba.growth_factor() == Approx(2.0));
    }
}
