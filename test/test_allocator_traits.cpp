#include "allocator/allocator_traits.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


struct dummy_allocator
{
    void*
    allocate_node(std::size_t size, std::size_t alignment)
    {
        fmt::print("dummy_allocator::allocate_node({}, {})\n", size, alignment);
        return nullptr;
    }

    void
    deallocate_node(void* ptr, std::size_t size, std::size_t alignment)
    {
        fmt::print("dummy_allocator::deallocate_node({}, {}, {})\n", ptr, size, alignment);
    }
};

TEST_CASE("allocator_traits", "[allocator_traits]")
{
    SECTION("ctor")
    {
        dummy_allocator alloc;
        allocator_traits<dummy_allocator> ac;

        REQUIRE(ac.allocate_node(alloc, 32, 8) == nullptr);
        REQUIRE(ac.allocate_array(alloc, 10, 32, 8) == nullptr);
        ac.deallocate_node(alloc, nullptr, 32, 8);
        ac.deallocate_array(alloc, nullptr, 10, 32, 8);
        REQUIRE(ac.max_node_size(alloc) == 18446744073709551615u);
        REQUIRE(ac.max_array_size(alloc) == 18446744073709551615u);
        REQUIRE(ac.max_alignment(alloc) == 16);
    }
}
