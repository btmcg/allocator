#include "memory_arena.hpp"
#include "util/assert.hpp"


memory_block
memory_arena::allocate_block() noexcept
{
    used_.push(alloc_.allocate_block());
    DEBUG_ASSERT(!used_.empty());
    return used_.top();
}

void
memory_arena::deallocate_block() noexcept
{
    alloc_.deallocate_block(used_.pop());
}
