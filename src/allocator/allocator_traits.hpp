#pragma once

#include <cstddef> // std::size_t


template <class Allocator>
class allocator_traits
{
public:
    static void*
    allocate_node(Allocator& alloc, std::size_t size, std::size_t alignment)
    {
        return alloc.allocate_node(size, alignment);
    }

    static void
    deallocate_node(Allocator& alloc, void* mem, std::size_t size, std::size_t alignment) noexcept
    {
        alloc.deallocate_node(mem, size, alignment);
    }

    static void*
    allocate_array(Allocator& alloc, std::size_t count, std::size_t size, std::size_t alignment)
    {
        return alloc.allocate_array(count, size, alignment);
    }

    static void
    deallocate_array(Allocator& alloc, void* mem, std::size_t count, std::size_t size, std::size_t alignment) noexcept
    {
        alloc.deallocate_array(mem, count, size, alignment);
    }

    static std::size_t
    max_node_size(Allocator const& alloc)
    {
        return alloc.max_node_size();
    }

    static std::size_t
    max_array_size(Allocator const& alloc)
    {
        return alloc.max_array_size();
    }

    static std::size_t
    max_alignment(Allocator const& alloc)
    {
        return alloc.max_alignment();
    }
};
