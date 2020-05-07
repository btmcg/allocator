#include "allocator/heap_allocator.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdint>
#include <memory> // std::allocator_traits
#include <new> // std::bad_alloc


struct dummy_functor
{
    static void*
    allocate(std::size_t size, std::size_t /*alignment*/) noexcept
    {
        fmt::print("malloc({})\n", size);
        return nullptr;
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
    {
        fmt::print("free({})\n", ptr);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<std::byte>>::max_size({});
    }
};

struct malloc_functor
{
    static void*
    allocate(std::size_t size, std::size_t /*alignment*/) noexcept
    {
        fmt::print("malloc({})\n", size);
        return std::malloc(size);
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
    {
        fmt::print("free({})\n", ptr);
        std::free(ptr);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<std::byte>>::max_size({});
    }
};

TEST_CASE("lowlevel_allocator", "[lowlevel_allocator]")
{
    SECTION("dummy")
    {
        lowlevel_allocator<dummy_functor> lla;
        REQUIRE_THROWS_AS(lla.allocate_node(32, 32), std::bad_alloc);
        lla.deallocate_node(nullptr, 32, 32);
        REQUIRE(lla.max_node_size() == 9223372036854775807);

        lowlevel_allocator<dummy_functor> lla2 = std::move(lla);
        REQUIRE_THROWS_AS(lla2.allocate_node(32, 32), std::bad_alloc);
        lla2.deallocate_node(nullptr, 32, 32);
        REQUIRE(lla2.max_node_size() == 9223372036854775807);
    }

    SECTION("malloc")
    {
        lowlevel_allocator<malloc_functor> lla;
        void* ptr = lla.allocate_node(32, 32);
        REQUIRE(ptr != nullptr);
        lla.deallocate_node(ptr, 32, 32);
        ptr = nullptr;
        REQUIRE(lla.max_node_size() == 9223372036854775807);

        lowlevel_allocator<malloc_functor> lla2 = std::move(lla);
        ptr = lla2.allocate_node(64, 32);
        REQUIRE(ptr != nullptr);
        lla2.deallocate_node(ptr, 64, 32);
        ptr = nullptr;
        REQUIRE(lla2.max_node_size() == 9223372036854775807);
    }
}
