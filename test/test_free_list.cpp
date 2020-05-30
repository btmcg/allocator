#include "allocator/free_list.hpp"
#include <catch2/catch.hpp>
#include <cstdint>


struct object
{
    int a, b, c;
};

struct big_object
{
    std::uint64_t a, b, c;
};

TEST_CASE("free_list", "[free_list]")
{
    REQUIRE(sizeof(object) == 12);
    REQUIRE(sizeof(big_object) == 24);

    SECTION("getters")
    {
        {
            free_list fl(sizeof(object));
            REQUIRE(fl.node_size() == sizeof(object));
            REQUIRE(fl.alignment() == 8);
            REQUIRE(fl.capacity_left() == 0);
            REQUIRE(fl.used() == 0);
            REQUIRE(fl.empty());
            void* mem = std::malloc(sizeof(object) * 5);
            fl.insert(mem, sizeof(object) * 5);
            REQUIRE(fl.capacity_left() == 5);
            REQUIRE(fl.used() == 0);
            REQUIRE_FALSE(fl.empty());
            std::free(mem);
        }

        {
            free_list fl(sizeof(big_object));
            REQUIRE(fl.node_size() == sizeof(big_object));
            REQUIRE(fl.alignment() == 16);
            REQUIRE(fl.capacity_left() == 0);
            REQUIRE(fl.used() == 0);
            REQUIRE(fl.empty());
            void* mem = std::malloc(sizeof(big_object) * 5);
            fl.insert(mem, sizeof(big_object) * 5);
            REQUIRE(fl.capacity_left() == 5);
            REQUIRE(fl.used() == 0);
            REQUIRE_FALSE(fl.empty());
            std::free(mem);
        }
    }

    SECTION("move ctor")
    {
        free_list fl1(sizeof(object));
        REQUIRE(fl1.node_size() == sizeof(object));
        REQUIRE(fl1.alignment() == 8);
        REQUIRE(fl1.capacity_left() == 0);
        REQUIRE(fl1.used() == 0);
        REQUIRE(fl1.empty());

        void* mem = std::malloc(sizeof(object) * 5);
        fl1.insert(mem, sizeof(object) * 5);
        REQUIRE(fl1.capacity_left() == 5);
        REQUIRE(fl1.used() == 0);
        REQUIRE_FALSE(fl1.empty());

        free_list fl2(std::move(fl1));
        REQUIRE(fl2.node_size() == sizeof(object));
        REQUIRE(fl2.alignment() == 8);
        REQUIRE(fl2.capacity_left() == 5);
        REQUIRE(fl2.used() == 0);
        REQUIRE_FALSE(fl2.empty());

        std::free(mem);
    }

    SECTION("move assignment")
    {
        free_list fl1(sizeof(object));
        REQUIRE(fl1.node_size() == sizeof(object));
        REQUIRE(fl1.alignment() == 8);
        REQUIRE(fl1.capacity_left() == 0);
        REQUIRE(fl1.used() == 0);
        REQUIRE(fl1.empty());

        void* mem = std::malloc(sizeof(object) * 5);
        fl1.insert(mem, sizeof(object) * 5);
        REQUIRE(fl1.capacity_left() == 5);
        REQUIRE(fl1.used() == 0);
        REQUIRE_FALSE(fl1.empty());

        free_list fl2(sizeof(big_object));
        REQUIRE(fl2.node_size() == sizeof(big_object));
        REQUIRE(fl2.alignment() == 16);
        REQUIRE(fl2.capacity_left() == 0);
        REQUIRE(fl2.used() == 0);
        REQUIRE(fl2.empty());

        fl2 = std::move(fl1);
        REQUIRE(fl2.node_size() == sizeof(object));
        REQUIRE(fl2.alignment() == 8);
        REQUIRE(fl2.capacity_left() == 5);
        REQUIRE(fl2.used() == 0);
        REQUIRE_FALSE(fl2.empty());

        std::free(mem);
    }

    SECTION("swap")
    {
        free_list fl1(sizeof(object));
        REQUIRE(fl1.node_size() == sizeof(object));
        REQUIRE(fl1.alignment() == 8);
        REQUIRE(fl1.capacity_left() == 0);
        REQUIRE(fl1.empty());
        void* mem1 = std::malloc(sizeof(object) * 10);
        fl1.insert(mem1, sizeof(object) * 10);
        REQUIRE(fl1.capacity_left() == 10);
        REQUIRE_FALSE(fl1.empty());

        free_list fl2(sizeof(big_object));
        REQUIRE(fl2.node_size() == sizeof(big_object));
        REQUIRE(fl2.alignment() == 16);
        REQUIRE(fl2.capacity_left() == 0);
        REQUIRE(fl2.empty());
        void* mem2 = std::malloc(sizeof(big_object) * 5);
        fl2.insert(mem2, sizeof(big_object) * 5);
        REQUIRE(fl2.capacity_left() == 5);
        REQUIRE_FALSE(fl2.empty());

        std::swap(fl1, fl2);

        // fl1 is now free_list of big_objects
        REQUIRE(fl1.node_size() == sizeof(big_object));
        REQUIRE(fl1.alignment() == 16);
        REQUIRE(fl1.capacity_left() == 5);
        REQUIRE_FALSE(fl1.empty());

        // fl2 is now free_list of objects
        REQUIRE(fl2.node_size() == sizeof(object));
        REQUIRE(fl2.alignment() == 8);
        REQUIRE(fl2.capacity_left() == 10);
        REQUIRE_FALSE(fl2.empty());

        std::free(mem1);
        std::free(mem2);
    }

    SECTION("alloc-dealloc node")
    {
        void* mem = std::malloc(sizeof(object) * 5);

        free_list fl(sizeof(object), mem, sizeof(object) * 5);
        REQUIRE(fl.capacity_left() == 5);

        // allocate 5 objects and set values
        object* objs[5] = {};
        for (int i = 0; i < 5; ++i) {
            objs[i] = static_cast<object*>(fl.allocate());
            REQUIRE(objs[i] != nullptr);
            objs[i]->a = objs[i]->b = objs[i]->c = i;
            REQUIRE(fl.capacity_left() == static_cast<std::size_t>(5 - i - 1));
        }
        REQUIRE(fl.capacity_left() == 0);

        // verify object values
        for (int i = 0; i < 5; ++i) {
            REQUIRE(objs[i]->a == i);
            REQUIRE(objs[i]->b == i);
            REQUIRE(objs[i]->c == i);
        }

        // deallocate 2 objs (out of order)
        fl.deallocate(objs[3]);
        REQUIRE(fl.capacity_left() == 1);
        fl.deallocate(objs[1]);
        REQUIRE(fl.capacity_left() == 2);

        objs[3] = static_cast<object*>(fl.allocate());
        objs[3]->a = objs[3]->b = objs[3]->c = 300;
        REQUIRE(fl.capacity_left() == 1);

        objs[1] = static_cast<object*>(fl.allocate());
        objs[1]->a = objs[1]->b = objs[1]->c = 100;
        REQUIRE(fl.capacity_left() == 0);

        // verify object values
        for (int i = 0; i < 5; ++i) {
            REQUIRE(objs[i]->a == ((i == 1 || i == 3) ? i * 100 : i));
            REQUIRE(objs[i]->b == ((i == 1 || i == 3) ? i * 100 : i));
            REQUIRE(objs[i]->c == ((i == 1 || i == 3) ? i * 100 : i));
        }

        // deallocate everything
        for (std::size_t i = 0; i < 5; ++i) {
            REQUIRE(fl.capacity_left() == i);
            fl.deallocate(objs[i]);
        }

        std::free(mem);
    }

    SECTION("alloc-dealloc array")
    {
        constexpr std::size_t mem_size = sizeof(big_object) * 50;

        free_list fl(sizeof(big_object));
        REQUIRE(fl.node_size() == sizeof(big_object));
        REQUIRE(fl.alignment() == 16);
        REQUIRE(fl.capacity_left() == 0);
        REQUIRE(fl.empty());

        void* mem = std::malloc(mem_size);
        fl.insert(mem, mem_size);
        REQUIRE(fl.capacity_left() == 50);
        REQUIRE_FALSE(fl.empty());

        // allocate 5 objects and set values
        big_object* objs = static_cast<big_object*>(fl.allocate(5 * sizeof(big_object)));
        for (std::uint64_t i = 0; i < 5; ++i)
            objs[i].a = objs[i].b = objs[i].c = i;
        REQUIRE(fl.capacity_left() == 45);

        // verify object values
        for (std::uint64_t i = 0; i < 5; ++i) {
            REQUIRE(objs[i].a == i);
            REQUIRE(objs[i].b == i);
            REQUIRE(objs[i].c == i);
        }

        // deallocate array
        fl.deallocate(objs, 5 * sizeof(big_object));
        REQUIRE(fl.capacity_left() == 50);

        std::free(mem);
    }
}
