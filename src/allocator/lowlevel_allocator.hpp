#pragma once

#include <cstdlib> // std::aligned_alloc
#include <memory>
#include <new>


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
            throw std::bad_alloc();
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
    allocate(std::size_t size, std::size_t /*alignment*/) noexcept
    {
        /// Allocates heap memory.
        /// This function is used by the \ref malloc_allocator to allocate the heap memory.
        /// It is not defined on a freestanding implementation, a definition must be provided by the
        /// library user. \requiredbe This function shall return a block of uninitialized memory
        /// that is aligned for \c max_align_t and has the given size. The size parameter will not
        /// be zero. It shall return a \c nullptr if no memory is available. It must be thread safe.
        /// \defaultbe On a hosted implementation this function uses OS specific facilities, \c
        /// std::malloc is used as fallback. \ingroup memory allocator
        return std::malloc(size);
    }

    static void
    deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
    {
        /// Deallocates heap memory.
        /// This function is used by the \ref malloc_allocator to allocate the heap memory.
        /// It is not defined on a freestanding implementation, a definition must be provided by the
        /// library user. \requiredbe This function gets a pointer from a previous call to \ref
        /// heap_alloc with the same size. It shall free the memory. The pointer will not be zero.
        /// It must be thread safe. \defaultbe On a hosted implementation this function uses OS
        /// specific facilities, \c std::free is used as fallback. \ingroup memory allocator
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
    allocate(std::size_t size, std::size_t alignment) noexcept
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
    allocate(std::size_t size, std::size_t alignment) noexcept
    {
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
