#include "memory_block_stack.hpp"


// whether or not an alignment is valid, i.e. a power of two not zero
constexpr bool
is_valid_alignment(std::size_t alignment) noexcept
{
    return alignment && (alignment & (alignment - 1)) == 0u;
}

// whether or not the pointer is aligned for given alignment
// alignment must be valid
bool
is_aligned(void* ptr, std::size_t alignment) noexcept
{
    DEBUG_ASSERT(is_valid_alignment(alignment));
    auto address = reinterpret_cast<std::uintptr_t>(ptr);
    return address % alignment == 0u;
}

memory_block_stack::memory_block_stack(memory_block_stack&& other) noexcept
        : head_(other.head_)
{
    other.head_ = nullptr;
}

memory_block_stack&
memory_block_stack::operator=(memory_block_stack&& other) noexcept
{
    memory_block_stack tmp(std::move(other));
    swap(*this, tmp);
    return *this;
}

void
memory_block_stack::push(allocated_mb block) noexcept
{
    DEBUG_ASSERT(is_aligned(block.memory, MaxAlignment));
    auto n = reinterpret_cast<node*>(block.memory);
    n->prev = head_;
    n->usable_size = block.size - Offset;
    head_ = n;
}
