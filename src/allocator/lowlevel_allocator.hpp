#pragma once

#include <sys/mman.h>
#include <cstdlib>   // ::posix_memalign
#include <exception> // std::terminate
#include <memory>
#include <new>


class lowlevel_allocator
{
public:
    constexpr lowlevel_allocator() noexcept = default;
    constexpr lowlevel_allocator(lowlevel_allocator&&) noexcept = default;
    constexpr ~lowlevel_allocator() noexcept = default;
    lowlevel_allocator(lowlevel_allocator const&) noexcept = delete;

    constexpr lowlevel_allocator&
    operator=(lowlevel_allocator&&) noexcept
    {
        return *this;
    }

    lowlevel_allocator& operator=(lowlevel_allocator const&) = delete;

    [[nodiscard]] void*
    allocate_node(std::size_t size, std::size_t /*alignment*/) const noexcept
    {
        void* memory
                = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (memory == nullptr)
            std::terminate();

        return memory;
    }

    void
    deallocate_node(void* ptr, std::size_t size, std::size_t /*alignment*/) const noexcept
    {
        ::munmap(ptr, size);
    }

    std::size_t
    max_node_size() const noexcept
    {
        return std::allocator_traits<std::allocator<char>>::max_size({});
    }
};
