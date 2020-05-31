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
            std::size_t block_size, LLAllocator alloc = LLAllocator()) noexcept
            : LLAllocator(std::move(alloc))
            , block_size_(block_size)
    {
        // empty
    }

    memory_block
    allocate_block()
    {
        void* memory = LLAllocator::allocate_node(block_size_, 8);
        memory_block block(memory, block_size_);
        block_size_ = std::size_t(block_size_ * growth_factor());
        return block;
    }

    void
    deallocate_block(memory_block block) noexcept
    {
        LLAllocator::deallocate_node(block.memory, block.size, 8);
    }

    std::size_t
    next_block_size() const noexcept
    {
        return block_size_;
    }

    LLAllocator&
    get_allocator() noexcept
    {
        return *this;
    }

    static float
    growth_factor() noexcept
    {
        static constexpr auto factor = static_cast<float>(Num) / Den;
        return factor;
    }

private:
    std::size_t block_size_ = 0;
};
