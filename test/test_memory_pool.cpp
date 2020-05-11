#include "allocator/allocator_storage.hpp"
#include "allocator/memory_pool.hpp"
#include "allocator/reference_storage.hpp"
#include "allocator/std_allocator.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstdint>
#include <list>
#include <string>
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
        constexpr std::size_t l_node_size = 16 + sizeof(int);

        memory_pool pool(l_node_size, 4096);
        std::list<int, std_allocator<int, memory_pool>> list(pool);
        list.push_back(0);
        list.emplace_back(1);
        list.push_back(2);
        list.emplace_back(3);
        list.push_back(4);
        list.emplace_back(5);
        list.push_back(6);
        list.emplace_back(7);
        list.push_back(8);
        list.emplace_back(9);

        for (int i = 0; auto itr : list)
            REQUIRE(itr == i++);
    }

    SECTION("unordered_map")
    {
        struct obj
        {
            std::string str;
            int i;
        };
        constexpr std::size_t um_node_size = 32 + sizeof(std::pair<const int, obj>);

        memory_pool pool(um_node_size, 4096);
        std::unordered_map<int, obj, std::hash<int>, std::equal_to<int>,
                std_allocator<std::pair<const int, obj>, memory_pool>>
                map(pool);

        map.emplace(0, obj{"zero", 0});
        map[1] = obj{"one", 1};
        map.emplace(2, obj{"two", 2});
        map[3] = obj{"three", 3};
        map.emplace(4, obj{"four", 4});
        map[5] = obj{"five", 5};
        map.emplace(6, obj{"six", 6});
        map[7] = obj{"seven", 7};
        map.emplace(8, obj{"eight", 8});
        map[9] = obj{"nine", 9};

        for (auto const& itr : map)
            REQUIRE(itr.first == itr.second.i);
    }
}
