#include "allocator/memory_pool.hpp"
#include "allocator/std_allocator.hpp"
#include <cstdlib>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>


struct object
{
    int a;
    double b;
    std::string c;
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
    constexpr std::size_t list_node_size = 16 + sizeof(object);
    memory_pool pool1(list_node_size, 1000);
    std::list<object, std_allocator<object>> list(pool1);


    constexpr std::size_t map_node_size = 16 + sizeof(std::pair<const int, object>);
    memory_pool pool2(map_node_size, 1000);
    std::unordered_map<int, object, std::hash<int>, std::equal_to<int>,
            std_allocator<std::pair<const int, object>>>
            map(pool2);


    constexpr std::size_t set_node_size = 16 + sizeof(object);
    memory_pool pool3(set_node_size, 1000);
    std::unordered_set<object, std::hash<object>, std::equal_to<object>, std_allocator<object>> set(
            pool3);


    return EXIT_SUCCESS;
}
