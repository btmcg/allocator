#pragma once

#include <cstddef> // std::byte
#include <cstdint>

// stores free blocks for a memory pool
// memory blocks are fragmented and stored in a list
// debug: fills memory and uses a bigger node_size for fence memory
class free_list
{
public:
    // minimum element size
    static constexpr auto min_element_size = sizeof(char*);
    // alignment
    static constexpr auto min_element_alignment = alignof(char*);

    //=== constructor ===//
    free_list(std::size_t node_size) noexcept;

    // calls other constructor plus insert
    free_list(std::size_t node_size, void* mem, std::size_t size) noexcept;

    free_list(free_list&& other) noexcept;
    ~free_list() noexcept = default;

    free_list& operator=(free_list&& other) noexcept;

    friend void swap(free_list& a, free_list& b) noexcept;

    //=== insert/allocation/deallocation ===//
    // inserts a new memory block, by splitting it up and setting the links
    // does not own memory!
    // mem must be aligned for alignment()
    // pre: size != 0
    void insert(void* mem, std::size_t size) noexcept;

    // returns the usable size
    // i.e. how many memory will be actually inserted and usable on a call to insert()
    std::size_t
    usable_size(std::size_t size) const noexcept
    {
        return size;
    }

    // returns a single block from the list
    // pre: !empty()
    void* allocate() noexcept;

    // returns a memory block big enough for n bytes
    // might fail even if capacity is sufficient
    void* allocate(std::size_t n) noexcept;

    // deallocates a single block
    void deallocate(void* ptr) noexcept;

    // deallocates multiple blocks with n bytes total
    void deallocate(void* ptr, std::size_t n) noexcept;

    //=== getter ===//
    std::size_t
    node_size() const noexcept
    {
        return node_size_;
    }

    // alignment of all nodes
    std::size_t alignment() const noexcept;

    // number of nodes remaining
    std::size_t
    capacity() const noexcept
    {
        return capacity_;
    }

    bool
    empty() const noexcept
    {
        return first_ == nullptr;
    }

private:
    void insert_impl(void* mem, std::size_t size) noexcept;

    char* first_;
    std::size_t node_size_, capacity_;
};
