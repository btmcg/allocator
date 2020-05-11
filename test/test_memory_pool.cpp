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
        class object
        {
        public:
            object(int a, std::uint64_t b, int c) noexcept
                    : a_(a)
                    , b_(b)
                    , c_(c)
            {}
            int
            get_a() const noexcept
            {
                return a_;
            }
            int
            get_b() const noexcept
            {
                return b_;
            }
            int
            get_c() const noexcept
            {
                return c_;
            }

        private:
            int a_;
            std::uint64_t b_;
            int c_;
        };
        constexpr std::size_t um_node_size = 32 + sizeof(std::pair<const int, object>);

        memory_pool pool(um_node_size, 4096);
        std::unordered_map<int, object, std::hash<int>, std::equal_to<int>,
                std_allocator<std::pair<const int, object>, memory_pool>>
                map(pool);

        for (int i = 0; i < 10; ++i)
            map.emplace(i, object(i, static_cast<std::uint64_t>(i), i));

        for (auto const& itr : map)
            REQUIRE(itr.first == itr.second.get_a());
    }
}
