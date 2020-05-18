#include "allocator/memory_pool.hpp"
#include "allocator/std_allocator.hpp"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <cstddef> // std::byte
#include <cstdlib> // std::malloc, std::free


class object
{
public:
    object(int a, double b, bool c) noexcept
            : a_(a)
            , b_(b)
            , c_(c)
    {}
    ~object() noexcept
    {
        a_ = 0;
        b_ = 0.0;
        c_ = false;
    }

    int
    get_a() const
    {
        return a_;
    }
    double
    get_b() const
    {
        return b_;
    }
    bool
    get_c() const
    {
        return c_;
    }

private:
    int a_ = 0;
    double b_ = 0.0;
    bool c_ = false;
};

TEST_CASE("std_allocator", "[std_allocator]")
{
    SECTION("ctor")
    {
        memory_pool pool(sizeof(int), 1000);
        std_allocator<int> sa(pool);
    }

    SECTION("copy ctor")
    {
        memory_pool pool1(sizeof(int), 1000);
        std_allocator<int> sa1(pool1);
        std_allocator<int> sa2(sa1);
        std_allocator<int> sa3 = sa2;

        memory_pool pool2(sizeof(std::uint64_t), 1000);
        std_allocator<int> sa4(pool2);
        sa3 = sa4;
    }

    SECTION("prop traits")
    {
        memory_pool pool(sizeof(int), 1000);
        std_allocator<int> sa1(pool);

        std_allocator<int> sa2 = sa1.select_on_container_copy_construction();
        REQUIRE(&sa1 != &sa2);
    }

    SECTION("alloc-dealloc")
    {
        memory_pool pool(sizeof(int), 1000);
        std_allocator<int> sa(pool);

        int* node_ptr = nullptr;
        node_ptr = sa.allocate(1);
        REQUIRE(node_ptr != nullptr);

        int* arr_ptr = nullptr;
        arr_ptr = sa.allocate(100);
        REQUIRE(arr_ptr != nullptr);

        sa.deallocate(node_ptr, 1);
        sa.deallocate(arr_ptr, 100);
    }

    SECTION("construct-destroy primitive")
    {
        memory_pool pool(sizeof(int), 1000);
        std_allocator<int> sa(pool);

        int* ptr = nullptr;
        ptr = sa.allocate(1);
        REQUIRE(ptr != nullptr);

        sa.construct(ptr, 42);
        REQUIRE(*ptr == 42);

        sa.destroy(ptr);

        sa.deallocate(ptr, 1);
    }

    SECTION("construct-destroy object")
    {
        memory_pool pool(sizeof(object), 1000);
        std_allocator<object> sa(pool);

        object* ptr = nullptr;
        ptr = sa.allocate(1);
        REQUIRE(ptr != nullptr);

        sa.construct(ptr, 42, 3.14, true);
        REQUIRE(ptr->get_a() == 42);
        REQUIRE(ptr->get_b() == Approx(3.14));
        REQUIRE(ptr->get_c());

        sa.destroy(ptr);
        // can be optimized out in -O3
        // REQUIRE(ptr->get_a() == 0);
        // REQUIRE(ptr->get_b() == Approx(0.0));
        // REQUIRE_FALSE(ptr->get_c());

        sa.deallocate(ptr, 1);
    }

    SECTION("get_allocator")
    {
        memory_pool pool(sizeof(object), 1000);
        std_allocator<object> sa(pool);

        memory_pool& mp_ref = sa.get_allocator();
        REQUIRE(&mp_ref == &pool);
        memory_pool const& mp_cref = sa.get_allocator();
        REQUIRE(&mp_cref == &pool);
    }

    SECTION("equality")
    {
        memory_pool obj_pool1(sizeof(object), 1000);
        memory_pool obj_pool2(sizeof(object), 1000);
        memory_pool int_pool1(sizeof(int), 1000);
        std_allocator<object> sa1(obj_pool1);
        std_allocator<object> sa2(obj_pool1);
        std_allocator<object> sa3(obj_pool2);
        std_allocator<object> sa4(int_pool1);

        // equality
        REQUIRE(sa1 == sa2);
        REQUIRE(sa2 == sa1);

        REQUIRE_FALSE(sa1 == sa3);
        REQUIRE_FALSE(sa1 == sa4);

        REQUIRE_FALSE(sa2 == sa3);
        REQUIRE_FALSE(sa2 == sa4);

        REQUIRE_FALSE(sa3 == sa1);
        REQUIRE_FALSE(sa3 == sa2);
        REQUIRE_FALSE(sa3 == sa4);

        REQUIRE_FALSE(sa4 == sa1);
        REQUIRE_FALSE(sa4 == sa2);
        REQUIRE_FALSE(sa4 == sa3);

        // inequality
        REQUIRE_FALSE(sa1 != sa2);
        REQUIRE_FALSE(sa2 != sa1);

        REQUIRE(sa1 != sa3);
        REQUIRE(sa1 != sa4);

        REQUIRE(sa2 != sa3);
        REQUIRE(sa2 != sa4);

        REQUIRE(sa3 != sa1);
        REQUIRE(sa3 != sa2);
        REQUIRE(sa3 != sa4);

        REQUIRE(sa4 != sa1);
        REQUIRE(sa4 != sa2);
        REQUIRE(sa4 != sa3);
    }
}
