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

    void*
    allocate_array(std::size_t count, std::size_t size, std::size_t alignment)
    {
        return allocate_node(count * size, alignment);
    }

    void
    deallocate_array(void* ptr, std::size_t count, std::size_t size, std::size_t alignment)
    {
        deallocate_node(ptr, count * size, alignment);
    }

    std::size_t
    max_node_size() const
    {
        return 64;
    }

    std::size_t
    max_array_size() const
    {
        return max_node_size();
    }

    std::size_t
    max_alignment() const
    {
        return 8;
    }
};

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


TEST_CASE("allocator_traits", "[allocator_traits]")
{
    SECTION("dummy")
    {
        dummy_allocator alloc;
        allocator_traits<dummy_allocator> ac;

        REQUIRE(ac.allocate_node(alloc, 32, 8) == nullptr);
        REQUIRE(ac.allocate_array(alloc, 10, 32, 8) == nullptr);
        ac.deallocate_node(alloc, nullptr, 32, 8);
        ac.deallocate_array(alloc, nullptr, 10, 32, 8);
        REQUIRE(ac.max_node_size(alloc) == 64);
        REQUIRE(ac.max_array_size(alloc) == 64);
        REQUIRE(ac.max_alignment(alloc) == 8);
    }

    SECTION("copying")
    {
        dummy_allocator alloc;
        allocator_traits<dummy_allocator> ac;
        REQUIRE(ac.allocate_node(alloc, 32, 8) == nullptr);
        REQUIRE(ac.allocate_array(alloc, 10, 32, 8) == nullptr);

        allocator_traits<dummy_allocator> ac2 = ac;
        REQUIRE(ac2.allocate_node(alloc, 32, 8) == nullptr);
        REQUIRE(ac2.allocate_array(alloc, 10, 32, 8) == nullptr);
    }

    SECTION("malloc")
    {
        printing_allocator alloc;
        allocator_traits<printing_allocator> ac;

        void* node_ptr = ac.allocate_node(alloc, 32, 8);
        REQUIRE(node_ptr != nullptr);

        void* arr_ptr = ac.allocate_array(alloc, 10, 32, 8);
        REQUIRE(arr_ptr != nullptr);

        ac.deallocate_node(alloc, node_ptr, 32, 8);
        ac.deallocate_array(alloc, arr_ptr, 10, 32, 8);

        REQUIRE(ac.max_node_size(alloc) == 64);
        REQUIRE(ac.max_array_size(alloc) == 640);
        REQUIRE(ac.max_alignment(alloc) == 8);
    }
}
