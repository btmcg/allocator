#include "allocator/block_allocator.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


TEST_CASE("block_allocator", "[block_allocator]")
{
    SECTION("ctor")
    {
        block_allocator ba1(4096);
        REQUIRE(ba1.growth_factor() == Approx(2.0));
        REQUIRE(ba1.next_block_size() == 4096);

        block_allocator ba2(1024);
        REQUIRE(ba2.growth_factor() == Approx(2.0));
        REQUIRE(ba2.next_block_size() == 1024);
    }

    SECTION("alloc-dealloc")
    {
        block_allocator ba(1024);
        REQUIRE(ba.growth_factor() == Approx(2.0));

        REQUIRE(ba.next_block_size() == 1024);
        memory_block mb1 = ba.allocate_block();
        REQUIRE(mb1.memory != nullptr);
        REQUIRE(mb1.size == 1024);

        REQUIRE(ba.next_block_size() == 2048);
        memory_block mb2 = ba.allocate_block();
        REQUIRE(mb2.memory != nullptr);
        REQUIRE(mb2.memory != mb1.memory);
        REQUIRE(mb2.size == 2048);

        ba.deallocate_block(mb1);
        ba.deallocate_block(mb2);
    }

    SECTION("constexpr")
    {
        constexpr block_allocator ba(1024);
        constexpr memory_block mb;
        ba.deallocate_block(mb);
        REQUIRE(ba.next_block_size() == 1024);
        REQUIRE(ba.growth_factor() == Approx(2.0));
    }
}
