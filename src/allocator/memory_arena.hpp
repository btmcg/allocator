#pragma once

#include "memory_block.hpp"
#include "memory_block_stack.hpp"
#include "common/assert.hpp"
#include <cstdint>
#include <utility>


template <typename BlockAllocator>
class memory_arena : private BlockAllocator
{
public:
    using allocator_type = BlockAllocator;

    template <typename... Args>
    explicit memory_arena(std::size_t block_size, Args&&... args)
            : allocator_type(block_size, std::forward<Args>(args)...)
    {
        // empty
    }

    /// Deallocates all memory blocks that where requested back
    /// to the BlockAllocator.
    ~memory_arena() noexcept
    {
        while (!used_.empty())
            allocator_type::deallocate_block(used_.pop());
    }

    memory_arena(memory_arena&& other) noexcept
            : allocator_type(std::move(other))
            , used_(std::move(other.used_))
    {}

    memory_arena&
    operator=(memory_arena&& other) noexcept
    {
        memory_arena tmp(std::move(other));
        swap(*this, tmp);
        return *this;
    }

    friend void
    swap(memory_arena& a, memory_arena& b) noexcept
    {
        std::swap(static_cast<allocator_type&>(a), static_cast<allocator_type&>(b));
        std::swap(a.used_, b.used_);
    }

    memory_block
    allocate_block()
    {
        used_.push(allocator_type::allocate_block());

        auto block = used_.top();
        return block;
    }

    memory_block
    current_block() const noexcept
    {
        return used_.top();
    }

    void
    deallocate_block() noexcept
    {
        get_allocator().deallocate_block(used_.pop());
    }

    bool
    owns(const void* ptr) const noexcept
    {
        return used_.owns(ptr);
    }

    std::size_t
    capacity() const noexcept
    {
        return size();
    }

    std::size_t
    size() const noexcept
    {
        return used_.size();
    }

    std::size_t
    next_block_size() const noexcept
    {
        return allocator_type::next_block_size() - memory_block_stack::implementation_offset;
    }

    allocator_type&
    get_allocator() noexcept
    {
        return *this;
    }

private:
    memory_block_stack used_;
};
