#pragma once

#include "memory_block.hpp"
#include <sys/mman.h>
#include <cstddef> // std::size_t
#include <cstdint>
#include <exception> // std::terminate


template <std::uint16_t Num = 2, std::uint16_t Den = 1>
class growing_block_allocator
{
    static_assert(static_cast<float>(Num) / Den >= 1.0, "invalid growth factor");

private:
    static constexpr std::size_t DefaultAlignment = 8;

private:
    std::size_t block_size_ = 0;

public:
    constexpr explicit growing_block_allocator(std::size_t block_size) noexcept;
    memory_block allocate_block() noexcept;
    constexpr void deallocate_block(memory_block block) const noexcept;
    constexpr std::size_t next_block_size() const noexcept;
    constexpr float growth_factor() const noexcept;
};

/**********************************************************************/

template <std::uint16_t Num, std::uint16_t Den>
constexpr growing_block_allocator<Num, Den>::growing_block_allocator(
        std::size_t block_size) noexcept
        : block_size_(block_size)
{
    // empty
}

template <std::uint16_t Num, std::uint16_t Den>
memory_block
growing_block_allocator<Num, Den>::allocate_block() noexcept
{
    void* memory = ::mmap(
            nullptr, block_size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == nullptr)
        std::terminate();

    memory_block block(memory, block_size_);
    block_size_ = static_cast<std::size_t>(block_size_ * growth_factor());
    return block;
}

template <std::uint16_t Num, std::uint16_t Den>
constexpr void
growing_block_allocator<Num, Den>::deallocate_block(memory_block block) const noexcept
{
    ::munmap(block.memory, block.size);
}

template <std::uint16_t Num, std::uint16_t Den>
constexpr std::size_t
growing_block_allocator<Num, Den>::next_block_size() const noexcept
{
    return block_size_;
}

template <std::uint16_t Num, std::uint16_t Den>
constexpr float
growing_block_allocator<Num, Den>::growth_factor() const noexcept
{
    return static_cast<float>(Num) / Den;
}
