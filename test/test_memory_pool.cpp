#include "allocator/allocator_storage.hpp"
#include "allocator/memory_pool.hpp"
#include "allocator/reference_storage.hpp"
#include "allocator/std_allocator.hpp"
#include <catch2/catch.hpp>
#include <cstdint>
#include <list>
#include <unordered_map>


struct object
{
    int a, b, c;
};

TEST_CASE("memory_pool", "[memory_pool]")
{
    SECTION("ctor")
    {
        constexpr std::size_t node_size = sizeof(std::uint64_t);
        constexpr std::size_t block_size = 4096;

        memory_pool pool(node_size, block_size);
        REQUIRE(pool.node_size() == node_size);
        REQUIRE(pool.capacity_left() == block_size - 16);
        REQUIRE(pool.next_capacity() == (block_size * 2) - 16);
    }

    SECTION("move ctor")
    {
        constexpr std::size_t node_size = sizeof(std::uint64_t);
        constexpr std::size_t block_size = 4096;

        memory_pool pool1(node_size, block_size);
        REQUIRE(pool1.node_size() == node_size);
        REQUIRE(pool1.capacity_left() == block_size - 16);
        REQUIRE(pool1.next_capacity() == (block_size * 2) - 16);

        memory_pool pool2(std::move(pool1));
        REQUIRE(pool2.node_size() == node_size);
        REQUIRE(pool2.capacity_left() == block_size - 16);
        REQUIRE(pool2.next_capacity() == (block_size * 2) - 16);
    }

    SECTION("move assignment")
    {
        constexpr std::size_t node_size = sizeof(std::uint64_t);
        constexpr std::size_t block_size = 4096;

        memory_pool pool1(node_size, block_size);
        REQUIRE(pool1.node_size() == node_size);
        REQUIRE(pool1.capacity_left() == block_size - 16);
        REQUIRE(pool1.next_capacity() == (block_size * 2) - 16);

        memory_pool pool2(node_size * 2, block_size * 2);
        pool2 = std::move(pool1);
        REQUIRE(pool2.node_size() == node_size);
        REQUIRE(pool2.capacity_left() == block_size - 16);
        REQUIRE(pool2.next_capacity() == (block_size * 2) - 16);
    }

    SECTION("alloc-dealloc")
    {
        memory_pool pool(sizeof(object), 4096);
        REQUIRE(pool.node_size() == sizeof(object));
        REQUIRE(pool.capacity_left() == 4096 - 16);
        REQUIRE(pool.next_capacity() == (4096 * 2) - 16);

        object* ptr1 = static_cast<object*>(pool.allocate_node());
        ptr1->a = ptr1->b = ptr1->c = 1;
        REQUIRE(pool.capacity_left() == (4096 - 16) - sizeof(object));

        pool.deallocate_node(ptr1);
        REQUIRE(pool.capacity_left() == 4096 - 16);

        object* ptr2 = static_cast<object*>(pool.allocate_node());
        REQUIRE(ptr1 == ptr2);
        REQUIRE(pool.capacity_left() == (4096 - 16) - sizeof(object));

        ptr1 = static_cast<object*>(pool.allocate_node());
        object* ptr3 = static_cast<object*>(pool.allocate_node());
        REQUIRE(pool.capacity_left() == (4096 - 16) - (sizeof(object) * 3));

        pool.deallocate_node(ptr1);
        pool.deallocate_node(ptr2);
        pool.deallocate_node(ptr3);
        REQUIRE(pool.capacity_left() == 4096 - 16);
    }

    SECTION("int list")
    {
        memory_pool pool(sizeof(int) + 16, 4096);
        std::list<int, std_allocator<int, memory_pool>> list(pool);
        list.push_back(0);
        list.push_back(1);
        list.push_back(2);
        list.emplace_back(3);

        int i = 0;
        for (auto itr : list)
            REQUIRE(itr == i++);
    }

    SECTION("unordered_map")
    {
        memory_pool pool(sizeof(std::pair<const int, double>) + 32, 400 * 1024);
        std::unordered_map<int, double, std::hash<int>, std::equal_to<int>,
                std_allocator<std::pair<const int, double>, memory_pool>>
                map(pool);

        // map.emplace(0, 0.0);
        // map.emplace(1, 10.0);
        // map.emplace(2, 20.0);
        // map.emplace(3, 30.0);
        // map.emplace(4, 40.0);
        map[0] = 0.0;
        map[1] = 10.0;
        map[2] = 20.0;
        map[3] = 30.0;
        map[4] = 40.0;
    }
}
