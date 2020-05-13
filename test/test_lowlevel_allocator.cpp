#include "allocator/lowlevel_allocator.hpp"
#include "common/compiler.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdint>
#include <memory> // std::allocator_traits


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
        REQUIRE_THROWS_AS(lla.allocate_node(32, 32), bad_allocation);
        lla.deallocate_node(nullptr, 32, 32);
#if (defined(COMPILER_GCC))
        REQUIRE(lla.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla.max_node_size() == 0xffffffffffffffff);
#endif

        lowlevel_allocator<dummy_functor> lla2 = std::move(lla);
        REQUIRE_THROWS_AS(lla2.allocate_node(32, 32), bad_allocation);
        lla2.deallocate_node(nullptr, 32, 32);
#if (defined(COMPILER_GCC))
        REQUIRE(lla2.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla2.max_node_size() == 0xffffffffffffffff);
#endif
    }

    SECTION("malloc")
    {
        lowlevel_allocator<malloc_functor> lla;
        void* ptr = lla.allocate_node(32, 32);
        REQUIRE(ptr != nullptr);
        lla.deallocate_node(ptr, 32, 32);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla.max_node_size() == 0xffffffffffffffff);
#endif

        lowlevel_allocator<malloc_functor> lla2 = std::move(lla);
        ptr = lla2.allocate_node(64, 32);
        REQUIRE(ptr != nullptr);
        lla2.deallocate_node(ptr, 64, 32);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla2.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla2.max_node_size() == 0xffffffffffffffff);
#endif
    }
}

TEST_CASE("malloc_allocator", "[malloc_allocator]")
{
    SECTION("alloc-dealloc")
    {
        lowlevel_allocator<malloc_allocator> lla;
        void* ptr = lla.allocate_node(32, 8);
        REQUIRE(ptr != nullptr);
        lla.deallocate_node(ptr, 32, 8);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla.max_node_size() == 0xffffffffffffffff);
#endif

        lowlevel_allocator<malloc_allocator> lla2 = std::move(lla);
        ptr = lla2.allocate_node(64, 8);
        REQUIRE(ptr != nullptr);
        lla2.deallocate_node(ptr, 64, 8);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla2.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla2.max_node_size() == 0xffffffffffffffff);
#endif
    }
}

TEST_CASE("new_allocator", "[new_allocator]")
{
    SECTION("alloc-dealloc")
    {
        lowlevel_allocator<new_allocator> lla;
        void* ptr = lla.allocate_node(32, 8);
        REQUIRE(ptr != nullptr);
        lla.deallocate_node(ptr, 32, 8);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla.max_node_size() == 0xffffffffffffffff);
#endif

        lowlevel_allocator<new_allocator> lla2 = std::move(lla);
        ptr = lla2.allocate_node(64, 8);
        REQUIRE(ptr != nullptr);
        lla2.deallocate_node(ptr, 64, 8);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla2.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla2.max_node_size() == 0xffffffffffffffff);
#endif
    }
}

TEST_CASE("posix_allocator", "[posix_allocator]")
{
    SECTION("alloc-dealloc")
    {
        lowlevel_allocator<posix_allocator> lla;
        void* ptr = lla.allocate_node(32, 8);
        REQUIRE(ptr != nullptr);
        lla.deallocate_node(ptr, 32, 8);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla.max_node_size() == 0xffffffffffffffff);
#endif

        lowlevel_allocator<posix_allocator> lla2 = std::move(lla);
        ptr = lla2.allocate_node(64, 8);
        REQUIRE(ptr != nullptr);
        lla2.deallocate_node(ptr, 64, 8);
        ptr = nullptr;
#if (defined(COMPILER_GCC))
        REQUIRE(lla2.max_node_size() == 0x7fffffffffffffff);
#elif (defined(COMPILER_CLANG))
        REQUIRE(lla2.max_node_size() == 0xffffffffffffffff);
#endif
    }

    SECTION("allocate non-multiple of 8")
    {
        lowlevel_allocator<posix_allocator> lla;
        void* ptr = lla.allocate_node(1, 8);
        REQUIRE(ptr != nullptr);

        ptr = lla.allocate_node(3, 8);
        REQUIRE(ptr != nullptr);

        ptr = lla.allocate_node(14, 8);
        REQUIRE(ptr != nullptr);
        ptr = lla.allocate_node(31, 8);
        REQUIRE(ptr != nullptr);

        // REQUIRE_THROWS_AS(lla.allocate_node(14, 8), bad_allocation);
        // REQUIRE_THROWS_AS(lla.allocate_node(31, 8), bad_allocation);
    }
}
