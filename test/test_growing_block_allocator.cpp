#include "allocator/growing_block_allocator.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


struct printing_allocator
{
    static std::size_t const alignment = 8;
    static std::size_t const node_size = 64;

    static void*
    allocate_node(std::size_t size, std::size_t alignment)
    {
        fmt::print("printing_allocator::allocate_node({}, {})\n", size, alignment);
        return std::malloc(size);
    }

    static void*
    allocate_array(std::size_t count, std::size_t size, std::size_t alignment)
    {
        fmt::print("printing_allocator::allocate_array({}, {}, {})\n", count, size, alignment);
        return std::malloc(size * count);
    }

    static void
    deallocate_node(void* ptr, std::size_t size, std::size_t alignment)
    {
        fmt::print("printing_allocator::deallocate_node({}, {}, {})\n", ptr, size, alignment);
        std::free(ptr);
    }

    static void
    deallocate_array(void* ptr, std::size_t count, std::size_t size, std::size_t alignment)
    {
        fmt::print("printing_allocator::deallocate_array({}, {}, {}, {})\n", ptr, count, size,
                alignment);
        std::free(ptr);
    }

    static std::size_t
    max_alignment()
    {
        fmt::print("printing_allocator::max_alignment()\n");
        return alignment;
    }

    static std::size_t
    max_node_size()
    {
        fmt::print("printing_allocator::max_node_size()\n");
        return node_size;
    }

    static std::size_t
    max_array_size()
    {
        fmt::print("printing_allocator::max_array_size()\n");
        return node_size * 10;
    }
};

TEST_CASE("growing_block_allocator", "[growing_block_allocator]")
{
    SECTION("ctor")
    {
        growing_block_allocator<> gba1(4096);
        REQUIRE(gba1.growth_factor() == Approx(2.0));
        REQUIRE(gba1.next_block_size() == 4096);

        growing_block_allocator<lowlevel_allocator, 3, 2> gba2(1024);
        REQUIRE(gba2.growth_factor() == Approx(1.5));
        REQUIRE(gba2.next_block_size() == 1024);
    }

    SECTION("alloc-dealloc")
    {
        growing_block_allocator<printing_allocator> gba(1024);
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
        constexpr growing_block_allocator<lowlevel_allocator, 2, 1> gba(1024);
        constexpr memory_block mb;
        gba.deallocate_block(mb);
        REQUIRE(gba.next_block_size() == 1024);
        REQUIRE(gba.growth_factor() == Approx(2.0));
    }
}
