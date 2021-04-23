#pragma once

#include "detail.hpp"
#include "util/assert.hpp"
#include <algorithm> // std::max
#include <climits>   // CHAR_BIT
#include <cstddef>   // std::max_align_t, std::size_t
#include <cstdint>
#include <cstring> // std::memcpy
#include <utility>


namespace detail {
    void set_int(void* address, std::uintptr_t i) noexcept;
    std::uintptr_t get_int(void* address) noexcept;

    // pointer to integer
    std::uintptr_t to_int(std::uint8_t* ptr) noexcept;

    // integer to pointer
    std::uint8_t* from_int(std::uintptr_t i) noexcept;
    std::uint8_t* list_get_next(void* address) noexcept;

    // stores a pointer value
    void list_set_next(void* address, std::uint8_t* ptr) noexcept;

    // i.e. array
    struct interval
    {
        std::uint8_t* prev = nullptr;  // last before
        std::uint8_t* first = nullptr; // first in
        std::uint8_t* last = nullptr;  // last in
        std::uint8_t* next = nullptr;  // first after

        // number of nodes in the interval
        constexpr std::size_t
        size(std::size_t node_size) const noexcept
        {
            // last is inclusive, so add actual_size to it
            // note: cannot use next, might not be directly after
            auto end = last + node_size;
            DEBUG_ASSERT((end - first) % node_size == 0u);
            return (end - first) / node_size;
        }
    };

    // - searches for n consecutive bytes
    // - begin and end are the proxy nodes
    // - assumes list is not empty
    // - similar to list_search_array()
    interval list_search_array(
            std::uint8_t* first, std::size_t bytes_needed, std::size_t node_size) noexcept;
} // namespace detail


class free_list
{
public:
    constexpr free_list(std::size_t node_size) noexcept;
    constexpr ~free_list() noexcept = default;
    free_list(free_list const&) noexcept = delete;
    free_list& operator=(free_list const&) noexcept = delete;
    constexpr free_list(free_list&&) noexcept;
    constexpr free_list& operator=(free_list&&) noexcept;

    void insert(void* mem, std::size_t size) noexcept;
    void* allocate() noexcept;
    void* allocate(std::size_t n) noexcept;
    void deallocate(void* ptr) noexcept;
    void deallocate(void* ptr, std::size_t n) noexcept;

    constexpr std::size_t node_size() const noexcept;
    constexpr std::size_t capacity_left() const noexcept;
    constexpr std::size_t used() const noexcept;
    constexpr std::size_t max_used() const noexcept;
    constexpr bool empty() const noexcept;

    friend constexpr void
    swap(free_list& a, free_list& b) noexcept
    {
        std::swap(a.first_, b.first_);
        std::swap(a.node_size_, b.node_size_);
        std::swap(a.capacity_left_, b.capacity_left_);
        std::swap(a.used_, b.used_);
        std::swap(a.max_used_, b.max_used_);
    }

private:
    void insert_impl(void* mem, std::size_t size) noexcept;

private:
    std::uint8_t* first_ = nullptr;
    std::size_t node_size_ = 0;
    std::size_t capacity_left_ = 0;
    std::size_t used_ = 0;
    std::size_t max_used_ = 0; ///< only for stats tracking
};

/**********************************************************************/

constexpr free_list::free_list(std::size_t node_size) noexcept
        : node_size_(node_size > detail::MinElementSize
                        ? detail::round_up_to_align(node_size, detail::DefaultAlignment)
                        : detail::MinElementSize)
{
    // empty
}

constexpr free_list::free_list(free_list&& other) noexcept
        : first_(other.first_)
        , node_size_(other.node_size_)
        , capacity_left_(other.capacity_left_)
        , used_(other.used_)
        , max_used_(other.max_used_)
{
    other.first_ = nullptr;
    other.capacity_left_ = 0;
    other.used_ = 0;
    other.max_used_ = 0;
}

constexpr free_list&
free_list::operator=(free_list&& other) noexcept
{
    free_list tmp(std::move(other));
    swap(*this, tmp);
    return *this;
}

constexpr std::size_t
free_list::node_size() const noexcept
{
    return node_size_;
}

constexpr std::size_t
free_list::capacity_left() const noexcept
{
    return capacity_left_;
}

constexpr std::size_t
free_list::used() const noexcept
{
    return used_;
}

constexpr std::size_t
free_list::max_used() const noexcept
{
    return max_used_;
}

constexpr bool
free_list::empty() const noexcept
{
    return first_ == nullptr;
}
