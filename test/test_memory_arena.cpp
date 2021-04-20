#include "allocator/growing_block_allocator.hpp"
#include "allocator/memory_arena.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


TEST_CASE("memory_arena", "[memory_arena]")
{
    SECTION("ctor")
    {
        memory_arena<growing_block_allocator<>> ma(1024);
        REQUIRE(ma.size() == 0);
    }

    SECTION("move ctor")
    {
        memory_arena<growing_block_allocator<>> ma1(1024);
        REQUIRE(ma1.size() == 0);

        memory_block mb = ma1.allocate_block();
        REQUIRE(mb.memory != nullptr);
        REQUIRE(mb.size == 1024 - 16);
        REQUIRE(ma1.current_block().memory == mb.memory);
        REQUIRE(ma1.current_block().size == mb.size);
        REQUIRE(ma1.size() == 1);

        memory_arena<growing_block_allocator<>> ma2(std::move(ma1));
        REQUIRE(ma2.current_block().memory == mb.memory);
        REQUIRE(ma2.current_block().size == mb.size);
        REQUIRE(ma2.size() == 1);

        ma2.deallocate_block();
    }

    SECTION("move assignment")
    {
        memory_arena<growing_block_allocator<>> ma1(1024);
        REQUIRE(ma1.size() == 0);

        memory_block mb = ma1.allocate_block();
        REQUIRE(mb.memory != nullptr);
        REQUIRE(mb.size == 1024 - 16);
        REQUIRE(ma1.current_block().memory == mb.memory);
        REQUIRE(ma1.current_block().size == mb.size);
        REQUIRE(ma1.size() == 1);

        memory_arena<growing_block_allocator<>> ma2(0);
        ma2 = std::move(ma1);
        REQUIRE(ma2.current_block().memory == mb.memory);
        REQUIRE(ma2.current_block().size == mb.size);
        REQUIRE(ma2.size() == 1);

        ma2.deallocate_block();
    }

    SECTION("swap")
    {
        memory_arena<growing_block_allocator<>> ma1(1024);
        REQUIRE(ma1.size() == 0);

        memory_block mb = ma1.allocate_block();
        REQUIRE(mb.memory != nullptr);
        REQUIRE(mb.size == 1024 - 16);
        REQUIRE(ma1.current_block().memory == mb.memory);
        REQUIRE(ma1.current_block().size == mb.size);
        REQUIRE(ma1.size() == 1);

        memory_arena<growing_block_allocator<>> ma2(0);
        REQUIRE(ma2.size() == 0);

        swap(ma1, ma2);

        REQUIRE(ma2.current_block().memory == mb.memory);
        REQUIRE(ma2.current_block().size == mb.size);
        REQUIRE(ma2.size() == 1);

        REQUIRE(ma1.size() == 0);

        ma2.deallocate_block();
    }

    SECTION("alloc-dealloc")
    {
        constexpr std::size_t block_size = 1024;

        memory_arena<growing_block_allocator<>> ma(block_size);
        REQUIRE(ma.size() == 0);

        memory_block mb1 = ma.allocate_block();
        REQUIRE(mb1.memory != nullptr);
        REQUIRE(mb1.size == block_size - 16);
        REQUIRE(ma.current_block().memory == mb1.memory);
        REQUIRE(ma.current_block().size == mb1.size);
        REQUIRE(ma.size() == 1);
        REQUIRE(ma.next_block_size() == (block_size * 2) - 16);

        memory_block mb2 = ma.allocate_block();
        REQUIRE(mb2.memory != nullptr);
        REQUIRE(mb2.size == (block_size * 2) - 16);
        REQUIRE(ma.current_block().memory == mb2.memory);
        REQUIRE(ma.current_block().size == mb2.size);
        REQUIRE(ma.size() == 2);
        REQUIRE(ma.next_block_size() == (block_size * 4) - 16);


        ma.deallocate_block();
        REQUIRE(ma.current_block().memory == mb1.memory);
        REQUIRE(ma.current_block().size == mb1.size);
        REQUIRE(ma.size() == 1);
        REQUIRE(ma.next_block_size() == (block_size * 4) - 16);

        ma.deallocate_block();
        REQUIRE(ma.size() == 0);
        REQUIRE(ma.next_block_size() == (block_size * 4) - 16);
    }

    SECTION("owns")
    {
        constexpr std::size_t block_size = 1024;

        memory_arena<growing_block_allocator<>> ma(block_size);
        REQUIRE(ma.size() == 0);

        memory_block mb = ma.allocate_block();
        REQUIRE(mb.memory != nullptr);
        REQUIRE(mb.size == block_size - 16);
        REQUIRE(ma.current_block().memory == mb.memory);
        REQUIRE(ma.current_block().size == mb.size);
        REQUIRE(ma.size() == 1);
        REQUIRE(ma.next_block_size() == (block_size * 2) - 16);

        REQUIRE(ma.owns(static_cast<std::byte*>(mb.memory) + 1));
        REQUIRE_FALSE(ma.owns(nullptr));
        REQUIRE_FALSE(ma.owns(static_cast<std::byte*>(mb.memory) - 17));

        ma.deallocate_block();
        REQUIRE(ma.size() == 0);
        REQUIRE(ma.next_block_size() == (block_size * 2) - 16);
        REQUIRE_FALSE(ma.owns(static_cast<std::byte*>(mb.memory) + 1));
        REQUIRE_FALSE(ma.owns(nullptr));
        REQUIRE_FALSE(ma.owns(static_cast<std::byte*>(mb.memory) - 17));
    }

    SECTION("constexpr")
    {
        constexpr memory_arena<growing_block_allocator<2, 1>> ma(1024);

        REQUIRE(ma.size() == 0);
        REQUIRE(ma.next_block_size() == 1024 - 16);
        REQUIRE_FALSE(ma.owns(nullptr));
    }
}
