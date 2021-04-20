#pragma once

#include "memory_block.hpp"
#include <sys/mman.h>
#include <cstddef> // std::size_t
#include <cstdint>
#include <exception> // std::terminate


class block_allocator
{
private:
    static constexpr double GrowthFactor = 2.0 / 1.0;
    static constexpr std::size_t DefaultAlignment = 8;

private:
    std::size_t block_size_ = 0;

public:
    constexpr explicit block_allocator(std::size_t block_size) noexcept
            : block_size_(block_size)
    {
        // empty
    }
    constexpr ~block_allocator() noexcept = default;
    constexpr block_allocator(block_allocator const&) noexcept = default;
    constexpr block_allocator& operator=(block_allocator const&) noexcept = default;
    constexpr block_allocator(block_allocator&&) noexcept = default;
    constexpr block_allocator& operator=(block_allocator&) noexcept = default;

    memory_block
    allocate_block() noexcept
    {
        void* memory = ::mmap(
                nullptr, block_size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (memory == nullptr)
            std::terminate();

        memory_block block(memory, block_size_);
        block_size_ = static_cast<std::size_t>(block_size_ * growth_factor());
        return block;
    }

    void
    deallocate_block(memory_block block) const noexcept
    {
        ::munmap(block.memory, block.size);
    }

    constexpr std::size_t
    next_block_size() const noexcept
    {
        return block_size_;
    }

    constexpr double
    growth_factor() const noexcept
    {
        return GrowthFactor;
    }
};
