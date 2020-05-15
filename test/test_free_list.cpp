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
            REQUIRE(fl.capacity() == 0);
            REQUIRE(fl.empty());
            void* mem = std::malloc(sizeof(object) * 5);
            fl.insert(mem, sizeof(object) * 5);
            REQUIRE(fl.capacity() == 5);
            REQUIRE_FALSE(fl.empty());
            std::free(mem);
        }

        {
            free_list fl(sizeof(big_object));
            REQUIRE(fl.node_size() == sizeof(big_object));
            REQUIRE(fl.alignment() == 16);
            REQUIRE(fl.capacity() == 0);
            REQUIRE(fl.empty());
            void* mem = std::malloc(sizeof(big_object) * 5);
            fl.insert(mem, sizeof(big_object) * 5);
            REQUIRE(fl.capacity() == 5);
            REQUIRE_FALSE(fl.empty());
            std::free(mem);
        }
    }

    SECTION("alloc-dealloc node")
    {
        void* mem = std::malloc(sizeof(object) * 5);

        free_list fl(sizeof(object), mem, sizeof(object) * 5);
        REQUIRE(fl.capacity() == 5);

        // allocate 5 objects and set values
        object* objs[5] = {};
        for (int i = 0; i < 5; ++i) {
            objs[i] = static_cast<object*>(fl.allocate());
            REQUIRE(objs[i] != nullptr);
            objs[i]->a = objs[i]->b = objs[i]->c = i;
            REQUIRE(fl.capacity() == static_cast<std::size_t>(5 - i - 1));
        }
        REQUIRE(fl.capacity() == 0);

        // verify object values
        for (int i = 0; i < 5; ++i) {
            REQUIRE(objs[i]->a == i);
            REQUIRE(objs[i]->b == i);
            REQUIRE(objs[i]->c == i);
        }

        // deallocate 2 objs (out of order)
        fl.deallocate(objs[3]);
        REQUIRE(fl.capacity() == 1);
        fl.deallocate(objs[1]);
        REQUIRE(fl.capacity() == 2);

        objs[3] = static_cast<object*>(fl.allocate());
        objs[3]->a = objs[3]->b = objs[3]->c = 300;
        REQUIRE(fl.capacity() == 1);

        objs[1] = static_cast<object*>(fl.allocate());
        objs[1]->a = objs[1]->b = objs[1]->c = 100;
        REQUIRE(fl.capacity() == 0);

        // verify object values
        for (int i = 0; i < 5; ++i) {
            REQUIRE(objs[i]->a == ((i == 1 || i == 3) ? i * 100 : i));
            REQUIRE(objs[i]->b == ((i == 1 || i == 3) ? i * 100 : i));
            REQUIRE(objs[i]->c == ((i == 1 || i == 3) ? i * 100 : i));
        }

        // deallocate everything
        for (std::size_t i = 0; i < 5; ++i) {
            REQUIRE(fl.capacity() == i);
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
        REQUIRE(fl.capacity() == 0);
        REQUIRE(fl.empty());

        void* mem = std::malloc(mem_size);
        fl.insert(mem, mem_size);
        REQUIRE(fl.capacity() == 50);
        REQUIRE_FALSE(fl.empty());

        // allocate 5 objects and set values
        big_object* objs = static_cast<big_object*>(fl.allocate(5 * sizeof(big_object)));
        for (std::uint64_t i = 0; i < 5; ++i)
            objs[i].a = objs[i].b = objs[i].c = i;
        REQUIRE(fl.capacity() == 45);

        // verify object values
        for (std::uint64_t i = 0; i < 5; ++i) {
            REQUIRE(objs[i].a == i);
            REQUIRE(objs[i].b == i);
            REQUIRE(objs[i].c == i);
        }

        // deallocate array
        fl.deallocate(objs, 5 * sizeof(big_object));
        REQUIRE(fl.capacity() == 50);

        std::free(mem);
    }
}
