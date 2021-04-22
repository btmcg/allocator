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
    constexpr explicit block_allocator(std::size_t block_size) noexcept;
    constexpr ~block_allocator() noexcept = default;
    constexpr block_allocator(block_allocator const&) noexcept = default;
    constexpr block_allocator& operator=(block_allocator const&) noexcept = default;
    constexpr block_allocator(block_allocator&&) noexcept = default;
    constexpr block_allocator& operator=(block_allocator&) noexcept = default;

    memory_block allocate_block() noexcept;
    void deallocate_block(memory_block block) const noexcept;
    constexpr std::size_t next_block_size() const noexcept;
    constexpr double growth_factor() const noexcept;
};

/**********************************************************************/

constexpr block_allocator::block_allocator(std::size_t block_size) noexcept
        : block_size_(block_size)
{
    // empty
}

constexpr std::size_t
block_allocator::next_block_size() const noexcept
{
    return block_size_;
}

constexpr double
block_allocator::growth_factor() const noexcept
{
    return GrowthFactor;
}
