#include "allocator/memory_pool.hpp"
#include "allocator/mp_allocator.hpp"
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
        constexpr std::size_t count = 100;

        memory_pool pool(node_size, count);
        REQUIRE(pool.node_size() == node_size);
        REQUIRE(pool.capacity_left() == node_size * count - 16);
        REQUIRE(pool.next_capacity() == (node_size * count * 2) - 16);
    }

    SECTION("move ctor")
    {
        constexpr std::size_t node_size = sizeof(std::uint64_t);
        constexpr std::size_t count = 1000;

        memory_pool pool1(node_size, count);
        REQUIRE(pool1.node_size() == node_size);
        REQUIRE(pool1.capacity_left() == node_size * count - 16);
        REQUIRE(pool1.next_capacity() == (node_size * count * 2) - 16);

        memory_pool pool2(std::move(pool1));
        REQUIRE(pool2.node_size() == node_size);
        REQUIRE(pool2.capacity_left() == node_size * count - 16);
        REQUIRE(pool2.next_capacity() == (node_size * count * 2) - 16);
    }

    SECTION("move assignment")
    {
        constexpr std::size_t node_size = sizeof(std::uint64_t);
        constexpr std::size_t count = 100;

        memory_pool pool1(node_size, count);
        REQUIRE(pool1.node_size() == node_size);
        REQUIRE(pool1.capacity_left() == node_size * count - 16);
        REQUIRE(pool1.next_capacity() == (node_size * count * 2) - 16);

        memory_pool pool2(node_size * 2, count * 2);
        pool2 = std::move(pool1);
        REQUIRE(pool2.node_size() == node_size);
        REQUIRE(pool2.capacity_left() == node_size * count - 16);
        REQUIRE(pool2.next_capacity() == (node_size * count * 2) - 16);
    }

    SECTION("alloc-dealloc")
    {
        constexpr std::size_t node_size = sizeof(object) + 4; // to make size 8-byte aligned
        constexpr std::size_t count = 100;

        memory_pool pool(node_size, count);
        REQUIRE(pool.node_size() == node_size);
        REQUIRE(pool.capacity_left() == node_size * count - 16);
        REQUIRE(pool.next_capacity() == (node_size * count * 2) - 16);

        object* ptr1 = static_cast<object*>(pool.allocate_node());
        ptr1->a = ptr1->b = ptr1->c = 1;
        REQUIRE(pool.capacity_left() == (node_size * count - 16) - node_size);

        pool.deallocate_node(ptr1);
        REQUIRE(pool.capacity_left() == node_size * count - 16);

        object* ptr2 = static_cast<object*>(pool.allocate_node());
        REQUIRE(ptr1 == ptr2);
        REQUIRE(pool.capacity_left() == (node_size * count - 16) - node_size);

        ptr1 = static_cast<object*>(pool.allocate_node());
        object* ptr3 = static_cast<object*>(pool.allocate_node());
        REQUIRE(pool.capacity_left() == (node_size * count - 16) - (node_size * 3));

        pool.deallocate_node(ptr1);
        pool.deallocate_node(ptr2);
        pool.deallocate_node(ptr3);
        REQUIRE(pool.capacity_left() == node_size * count - 16);
    }

    SECTION("int list")
    {
        constexpr std::size_t l_node_size = sizeof(std::int64_t) + (sizeof(std::uintptr_t) * 2);
        constexpr std::size_t l_count = 20;

        memory_pool pool(l_node_size, l_count);
        std::list<int, mp_allocator<int>> list(pool);
        list.push_back(0);
        list.push_back(1);
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

    SECTION("copy int list")
    {
        constexpr std::size_t l_node_size = sizeof(std::int64_t) + (sizeof(std::uintptr_t) * 2);
        constexpr std::size_t l_count = 30;

        memory_pool pool(l_node_size, l_count);
        std::list<int, mp_allocator<int>> list1(pool);
        list1.emplace_back(0);
        list1.emplace_back(1);
        list1.emplace_back(2);
        list1.emplace_back(3);
        list1.emplace_back(4);

        std::list<int, mp_allocator<int>> list2 = list1;

        auto l1_itr = list1.cbegin();
        for (int i = 0; i < 5; ++i) {
            REQUIRE(*l1_itr == i);
            ++l1_itr;
        }

        auto l2_itr = list2.cbegin();
        for (int i = 0; i < 5; ++i) {
            REQUIRE(*l2_itr == i);
            ++l2_itr;
        }

        list1.emplace_back(5);
        list1.emplace_back(6);
        list1.emplace_back(7);
        list1.emplace_back(8);
        list1.emplace_back(9);
        l1_itr = list1.cbegin();
        for (int i = 0; i < 10; ++i) {
            REQUIRE(*l1_itr == i);
            ++l1_itr;
        }

        // ensure list2 still ends at 5
        auto l2_end = list2.crend();
        REQUIRE(*l2_end == 5);
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
        constexpr std::size_t um_node_size = 16 + sizeof(std::pair<const int, object>);
        constexpr std::size_t um_count = 100;

        memory_pool pool(um_node_size, um_count);
        std::unordered_map<int, object, std::hash<int>, std::equal_to<int>,
                mp_allocator<std::pair<const int, object>>>
                map(pool);

        for (int i = 0; i < 10; ++i)
            map.emplace(i, object(i, static_cast<std::uint64_t>(i), i));

        for (auto const& itr : map)
            REQUIRE(itr.first == itr.second.get_a());
    }
}
