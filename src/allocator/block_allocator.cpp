#include "block_allocator.hpp"


memory_block
block_allocator::allocate_block() noexcept
{
    void* memory = ::mmap(
            nullptr, block_size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == nullptr)
        std::terminate();

    memory_block block(memory, block_size_);
    block_size_ = static_cast<std::size_t>(block_size_ * growth_factor());
    return block;
}

void
block_allocator::deallocate_block(memory_block block) const noexcept
{
    ::munmap(block.memory, block.size);
}
