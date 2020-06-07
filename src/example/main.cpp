#include "allocator/memory_pool.hpp"
#include "allocator/mp_allocator.hpp"
#include <cstdint>
#include <cstdlib>
#include <list>
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


    constexpr std::size_t map_node_size = 16 + sizeof(std::pair<const int, object>);
    memory_pool pool2(map_node_size, num_elements);
    std::unordered_map<int, object, std::hash<int>, std::equal_to<>,
            mp_allocator<std::pair<int const, object>>>
            map(pool2);


    constexpr std::size_t set_node_size = 16 + sizeof(object);
    memory_pool pool3(set_node_size, num_elements);
    std::unordered_set<object, std::hash<object>, std::equal_to<>, mp_allocator<object>> set(pool3);

    return EXIT_SUCCESS;
}
