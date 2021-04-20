#pragma once

#include "block_allocator.hpp"
#include "memory_block.hpp"
#include "memory_block_stack.hpp"
#include "util/assert.hpp"
#include <cstdint>
#include <utility>


class memory_arena
{
private:
    block_allocator alloc_;
    memory_block_stack used_;

public:
    constexpr explicit memory_arena(std::size_t block_size) noexcept
            : alloc_(block_size)
            , used_()
    {
        // empty
    }

    /// Returns all memory blocks back to the underlying allocator
    constexpr ~memory_arena() noexcept
    {
        while (!used_.empty())
            alloc_.deallocate_block(used_.pop());
    }

    // memory_arena(memory_arena const&) noexcept = default;
    // memory_arena& operator=(memory_arena const&) noexcept = default;
    memory_arena(memory_arena&& other) noexcept = default;
    memory_arena& operator=(memory_arena&& rhs) noexcept = default;

    memory_block
    allocate_block() noexcept
    {
        used_.push(alloc_.allocate_block());
        DEBUG_ASSERT(!used_.empty());
        return used_.top();
    }

    memory_block
    current_block() const noexcept
    {
        return used_.top();
    }

    void
    deallocate_block() noexcept
    {
        alloc_.deallocate_block(used_.pop());
    }

    constexpr bool
    owns(void const* ptr) const noexcept
    {
        return used_.owns(ptr);
    }

    constexpr std::size_t
    size() const noexcept
    {
        return used_.size();
    }

    constexpr std::size_t
    next_block_size() const noexcept
    {
        return alloc_.next_block_size() - memory_block_stack::Offset;
    }

    block_allocator&
    get_allocator() noexcept
    {
        return alloc_;
    }

    friend void
    swap(memory_arena& a, memory_arena& b) noexcept
    {
        std::swap(a.alloc_, b.alloc_);
        std::swap(a.used_, b.used_);
    }
};
