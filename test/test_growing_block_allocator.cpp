#include "allocator/memory_arena.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


TEST_CASE("growing_block_allocator", "[growing_block_allocator]")
{
    SECTION("ctor")
    {
        growing_block_allocator<> gba(4096);
    }
}
