#include "memory_arena.hpp"


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


constexpr std::size_t max_alignment = alignof(std::max_align_t);

/**********************************************************************/

constexpr std::size_t memory_block_stack::node::div_alignment
        = sizeof(memory_block_stack::node) / max_alignment;
constexpr std::size_t memory_block_stack::node::mod_offset
        = sizeof(memory_block_stack::node) % max_alignment != 0u;
constexpr std::size_t memory_block_stack::node::offset
        = (div_alignment + mod_offset) * max_alignment;

constexpr std::size_t memory_block_stack::implementation_offset = memory_block_stack::node::offset;

memory_block_stack::memory_block_stack() noexcept
        : head_(nullptr)
{}

memory_block_stack::~memory_block_stack() noexcept
{}

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

bool
memory_block_stack::empty() const noexcept
{
    return head_ == nullptr;
}

void
swap(memory_block_stack& a, memory_block_stack& b) noexcept
{
    std::swap(a.head_, b.head_);
}

void
memory_block_stack::push(allocated_mb block) noexcept
{
    DEBUG_ASSERT(is_aligned(block.memory, max_alignment));
    auto next = ::new (block.memory) node(head_, block.size - node::offset);
    head_ = next;
}

memory_block_stack::allocated_mb
memory_block_stack::pop() noexcept
{
    DEBUG_ASSERT(head_);
    auto to_pop = head_;
    head_ = head_->prev;
    return {to_pop, to_pop->usable_size + node::offset};
}

void
memory_block_stack::steal_top(memory_block_stack& other) noexcept
{
    DEBUG_ASSERT(other.head_);
    auto to_steal = other.head_;
    other.head_ = other.head_->prev;

    to_steal->prev = head_;
    head_ = to_steal;
}

bool
memory_block_stack::owns(const void* ptr) const noexcept
{
    auto address = static_cast<const char*>(ptr);
    for (auto cur = head_; cur; cur = cur->prev) {
        auto mem = static_cast<char*>(static_cast<void*>(cur));
        if (address >= mem && address < mem + cur->usable_size)
            return true;
    }
    return false;
}

std::size_t
memory_block_stack::size() const noexcept
{
    std::size_t res = 0u;
    for (auto cur = head_; cur; cur = cur->prev)
        ++res;
    return res;
}

memory_block_stack::inserted_mb
memory_block_stack::top() const noexcept
{
    DEBUG_ASSERT(head_);
    auto mem = static_cast<void*>(head_);
    return {static_cast<char*>(mem) + node::offset, head_->usable_size};
}
