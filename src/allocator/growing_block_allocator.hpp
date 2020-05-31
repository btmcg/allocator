#pragma once

#include "lowlevel_allocator.hpp"
#include "memory_block.hpp"
#include <cstddef> // std::size_t
#include <cstdint>


template <typename LLAllocator = lowlevel_allocator<malloc_allocator>, std::uint16_t Num = 2,
        std::uint16_t Den = 1>
class growing_block_allocator : private LLAllocator
{
    static_assert(static_cast<float>(Num) / Den >= 1.0, "invalid growth factor");

public:
    constexpr explicit growing_block_allocator(
            std::size_t block_size, LLAllocator alloc = LLAllocator()) noexcept;
    memory_block allocate_block();
    void deallocate_block(memory_block block) noexcept;
    std::size_t next_block_size() const noexcept;
    LLAllocator& get_allocator() noexcept;
    static float growth_factor() noexcept;

private:
    std::size_t block_size_ = 0;
};

/**********************************************************************/

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
constexpr growing_block_allocator<LLAllocator, Num, Den>::growing_block_allocator(
        std::size_t block_size, LLAllocator alloc) noexcept
        : LLAllocator(std::move(alloc))
        , block_size_(block_size)
{
    // empty
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
memory_block
growing_block_allocator<LLAllocator, Num, Den>::allocate_block()
{
    void* memory = LLAllocator::allocate_node(block_size_, 8);
    memory_block block(memory, block_size_);
    block_size_ = std::size_t(block_size_ * growth_factor());
    return block;
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
void
growing_block_allocator<LLAllocator, Num, Den>::deallocate_block(memory_block block) noexcept
{
    LLAllocator::deallocate_node(block.memory, block.size, 8);
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
std::size_t
growing_block_allocator<LLAllocator, Num, Den>::next_block_size() const noexcept
{
    return block_size_;
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
LLAllocator&
growing_block_allocator<LLAllocator, Num, Den>::get_allocator() noexcept
{
    return *this;
}

template <typename LLAllocator, std::uint16_t Num, std::uint16_t Den>
float
growing_block_allocator<LLAllocator, Num, Den>::growth_factor() noexcept
{
    static constexpr auto factor = static_cast<float>(Num) / Den;
    return factor;
}
