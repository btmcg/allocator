#include "allocator/memory_pool.hpp"
#include "allocator/mp_allocator.hpp"
#include <cstdint>
#include <cstdlib>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>


struct object
{
    int a;
    std::uint64_t b;
    std::string c;

    constexpr bool
    operator==(object const& rhs) const
    {
        return a == rhs.a && b == rhs.b && c == rhs.c;
    }
};

struct price_level
{
    std::uint32_t price;
    std::uint32_t qty;
};

namespace std {
    template <>
    struct hash<object>
    {
        std::size_t
        operator()(object const& o) const noexcept
        {
            return std::hash<int>{}(o.a);
        }
    };
} // namespace std

int
main(int, char**)
{
    constexpr std::size_t num_elements = 1000;

    constexpr std::size_t list_node_size = 16 + sizeof(object);
    memory_pool pool1(list_node_size, num_elements);
    std::list<object, mp_allocator<object>> list(pool1);


    constexpr std::size_t umap_node_size = 16 + sizeof(std::pair<int const, object>);
    memory_pool pool2(umap_node_size, num_elements);
    std::unordered_map<int, object, std::hash<int>, std::equal_to<>,
            mp_allocator<std::pair<int const, object>>>
            umap(pool2);

    constexpr std::size_t set_node_size = 16 + sizeof(object);
    memory_pool pool3(set_node_size, num_elements);
    std::unordered_set<object, std::hash<object>, std::equal_to<>, mp_allocator<object>> set(pool3);

    constexpr std::size_t map_node_size = 32 + sizeof(std::pair<int const, object>);
    memory_pool pool4(map_node_size, num_elements);
    std::map<std::uint32_t, price_level, std::less<>,
            mp_allocator<std::pair<std::uint32_t const, price_level>>>
            map(pool4);

    return EXIT_SUCCESS;
}
