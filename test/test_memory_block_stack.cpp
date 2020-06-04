#include "allocator/memory_block_stack.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <type_traits> // std::aligned_storage


TEST_CASE("memory_block_stack", "[memory_block_stack]")
{
    SECTION("ctor")
    {
        memory_block_stack mbs1;
        REQUIRE(mbs1.empty());
        REQUIRE(mbs1.size() == 0);
    }

    SECTION("move ctor")
    {
        memory_block mb(std::malloc(1024), 1024);

        memory_block_stack mbs;
        mbs.push(mb);
        REQUIRE_FALSE(mbs.empty());
        REQUIRE(mbs.size() == 1);

        memory_block_stack new_mbs(std::move(mbs));
        REQUIRE_FALSE(new_mbs.empty());
        REQUIRE(new_mbs.size() == 1);

        memory_block mb_popped = new_mbs.pop();
        REQUIRE(mb_popped.memory == mb.memory);
        REQUIRE(mb_popped.size == mb.size);

        std::free(mb.memory);
    }

    SECTION("assignment")
    {
        memory_block mb(std::malloc(1024), 1024);
        memory_block_stack mbs;
        mbs.push(mb);
        REQUIRE_FALSE(mbs.empty());
        REQUIRE(mbs.size() == 1);

        memory_block_stack new_mbs = std::move(mbs);
        REQUIRE_FALSE(new_mbs.empty());
        REQUIRE(new_mbs.size() == 1);

        memory_block mb_popped = new_mbs.pop();
        REQUIRE(mb_popped.memory == mb.memory);
        REQUIRE(mb_popped.size == mb.size);

        std::free(mb.memory);
    }

    SECTION("swap")
    {
        memory_block mb1(std::malloc(1024), 1024);
        memory_block_stack mbs1;
        mbs1.push(mb1);
        REQUIRE_FALSE(mbs1.empty());
        REQUIRE(mbs1.size() == 1);

        memory_block mb2(std::malloc(2048), 2048);
        memory_block_stack mbs2;
        mbs2.push(mb2);
        REQUIRE_FALSE(mbs2.empty());
        REQUIRE(mbs2.size() == 1);

        swap(mbs1, mbs2);

        memory_block mb1_popped = mbs1.pop();
        REQUIRE(mb1_popped.memory == mb2.memory);
        REQUIRE(mb1_popped.size == mb2.size);

        memory_block mb2_popped = mbs2.pop();
        REQUIRE(mb2_popped.memory == mb1.memory);
        REQUIRE(mb2_popped.size == mb1.size);

        std::free(mb1.memory);
        std::free(mb2.memory);
    }

    SECTION("top")
    {
        memory_block mb1(std::malloc(1024), 1024);

        memory_block_stack mbs;
        mbs.push(mb1);

        REQUIRE(mbs.top().memory == static_cast<std::byte*>(mb1.memory) + 16);
        REQUIRE(mbs.top().size == mb1.size - 16);

        memory_block mb2(std::malloc(1024), 1024);
        mbs.push(mb2);

        REQUIRE(mbs.top().memory == static_cast<std::byte*>(mb2.memory) + 16);
        REQUIRE(mbs.top().size == mb2.size - 16);

        mbs.pop();
        mbs.pop();

        std::free(mb1.memory);
        std::free(mb2.memory);
    }

    SECTION("owns")
    {
        memory_block mb(std::malloc(1024), 1024);

        memory_block_stack mbs;
        mbs.push(mb);

        REQUIRE(mbs.owns(static_cast<std::byte*>(mb.memory) + 1));
        REQUIRE_FALSE(mbs.owns(nullptr));
        REQUIRE_FALSE(mbs.owns(static_cast<std::byte*>(mb.memory) - 1));

        std::free(mb.memory);
    }

    SECTION("constexpr")
    {
        constexpr memory_block_stack mbs;
        REQUIRE(mbs.empty());
        REQUIRE(mbs.size() == 0);
        REQUIRE_FALSE(mbs.owns(nullptr));
    }
}
