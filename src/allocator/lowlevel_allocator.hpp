#pragma once

#include "common/assert.hpp"
#include <fmt/format.h>
#include <cerrno>
#include <cstdlib> // std::aligned_alloc
#include <memory>
#include <new>


class bad_allocation : public std::bad_alloc
{
public:
    bad_allocation(std::size_t size, std::size_t alignment) noexcept
    {
        fmt::print(stderr,
                "lowlevel_allocator bad allocation: size={}, alignment={}, errno={}, strerror={}\n",
                size, alignment, errno, std::strerror(errno));
    }

    char const*
    what() const noexcept override
    {
        return "lowlevel_allocator bad allocation";
    }
};


// Functor controls low-level allocation:
// static void* allocate(std::size_t size, std::size_t alignment);
// static void deallocate(void *memory, std::size_t size, std::size_t alignment);
// static std::size_t max_node_size();
template <class Functor>
class lowlevel_allocator
{
public:
    using is_stateful = std::false_type;

    lowlevel_allocator() noexcept
    {}
    lowlevel_allocator(lowlevel_allocator&&) noexcept
    {}
    ~lowlevel_allocator() noexcept
    {}

    lowlevel_allocator&
    operator=(lowlevel_allocator&&) noexcept
    {
        return *this;
    }

    [[nodiscard]] void*
    allocate_node(std::size_t size, std::size_t alignment)
    {
        auto actual_size = size;

        auto memory = Functor::allocate(actual_size, alignment);
        if (!memory) {
            throw bad_allocation(actual_size, alignment);
            // FOONATHAN_THROW(out_of_memory(Functor::info(), actual_size));
        }

        return memory;
    }

    void
    deallocate_node(void* node, std::size_t size, std::size_t alignment) noexcept
    {
        auto actual_size = size;

        auto memory = node;
        Functor::deallocate(memory, actual_size, alignment);
    }

    std::size_t
    max_node_size() const noexcept
    {
        return Functor::max_node_size();
    }
};

/// A stateless \concept{concept_rawallocator,RawAllocator} that allocates memory from the heap.
/// It uses the two functions \ref heap_alloc and \ref heap_dealloc for the allocation,
/// which default to \c std::malloc and \c std::free.
/// \ingroup memory allocator
struct malloc_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t /*alignment = 8*/) noexcept
    {
        return std::malloc(size);
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
    {
        std::free(ptr);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};

struct new_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t alignment = 8) noexcept
    {
        return ::operator new(size, static_cast<std::align_val_t>(alignment));
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t alignment) noexcept
    {
        ::operator delete(ptr, static_cast<std::align_val_t>(alignment));
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};

struct posix_allocator
{
    [[nodiscard]] static void*
    allocate(std::size_t size, std::size_t alignment = 8) noexcept
    {
        DEBUG_ASSERT(size % alignment == 0);
        return std::aligned_alloc(alignment, size);
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
    {
        std::free(ptr);
    }

    static std::size_t
    max_node_size() noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};
