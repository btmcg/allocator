#pragma once

#include "block_allocator.hpp"
#include "memory_block.hpp"
#include "memory_block_stack.hpp"
#include <cstdint>
#include <utility>


class memory_arena
{
private:
    block_allocator alloc_;
    memory_block_stack used_;

public:
    constexpr explicit memory_arena(std::size_t block_size) noexcept;

    /// Returns all memory blocks back to the underlying allocator
    constexpr ~memory_arena() noexcept;
    memory_arena(memory_arena const&) noexcept = delete;
    memory_arena& operator=(memory_arena const&) noexcept = delete;
    memory_arena(memory_arena&& other) noexcept = default;
    memory_arena& operator=(memory_arena&& rhs) noexcept = default;

    memory_block allocate_block() noexcept;
    void deallocate_block() noexcept;
    constexpr memory_block current_block() const noexcept;
    constexpr bool owns(void const* ptr) const noexcept;
    constexpr std::size_t size() const noexcept;
    constexpr std::size_t next_block_size() const noexcept;
    constexpr block_allocator& get_allocator() noexcept;
    friend constexpr void
    swap(memory_arena& a, memory_arena& b) noexcept
    {
        std::swap(a.alloc_, b.alloc_);
        std::swap(a.used_, b.used_);
    }
};

/**********************************************************************/

constexpr memory_arena::memory_arena(std::size_t block_size) noexcept
        : alloc_(block_size)
        , used_()
{
    // empty
}

constexpr memory_arena::~memory_arena() noexcept
{
    while (!used_.empty())
        alloc_.deallocate_block(used_.pop());
}

constexpr memory_block
memory_arena::current_block() const noexcept
{
    return used_.top();
}

constexpr bool
memory_arena::owns(void const* ptr) const noexcept
{
    return used_.owns(ptr);
}

constexpr std::size_t
memory_arena::size() const noexcept
{
    return used_.size();
}

constexpr std::size_t
memory_arena::next_block_size() const noexcept
{
    return alloc_.next_block_size() - memory_block_stack::Offset;
}

constexpr block_allocator&
memory_arena::get_allocator() noexcept
{
    return alloc_;
}
