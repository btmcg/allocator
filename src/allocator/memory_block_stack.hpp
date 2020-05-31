#pragma once

#include "memory_block.hpp"
#include <cstddef> // std::size_t


/// stores memory block in an intrusive linked list and allows LIFO
/// access
class memory_block_stack
{
public:
    memory_block_stack() noexcept;
    ~memory_block_stack() noexcept;
    memory_block_stack(memory_block_stack&& other) noexcept;
    memory_block_stack& operator=(memory_block_stack&& other) noexcept;
    friend void swap(memory_block_stack& a, memory_block_stack& b) noexcept;

    // the raw allocated block returned from an allocator
    using allocated_mb = memory_block;

    // the inserted block slightly smaller to allow for the fixup value
    using inserted_mb = memory_block;

    // how much an inserted block is smaller
    static const std::size_t implementation_offset;

    void push(allocated_mb block) noexcept;
    allocated_mb pop() noexcept;
    inserted_mb top() const noexcept;
    bool empty() const noexcept;
    bool owns(const void* ptr) const noexcept;

    // O(n) size
    std::size_t size() const noexcept;

private:
    struct node
    {
        node* prev = nullptr;
        std::size_t usable_size = 0;

        node(node* p, std::size_t size) noexcept;

        static const std::size_t div_alignment;
        static const std::size_t mod_offset;
        static const std::size_t offset;
    };

    node* head_ = nullptr;
};
