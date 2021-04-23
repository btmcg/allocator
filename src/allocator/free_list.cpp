#include "free_list.hpp"

namespace detail {
    void
    set_int(void* address, std::uintptr_t i) noexcept
    {
        DEBUG_ASSERT(address != nullptr);
        std::memcpy(address, &i, sizeof(std::uintptr_t));
    }

    std::uintptr_t
    get_int(void* address) noexcept
    {
        DEBUG_ASSERT(address != nullptr);
        std::uintptr_t res = 0;
        std::memcpy(&res, address, sizeof(std::uintptr_t));
        return res;
    }

    std::uintptr_t
    to_int(std::uint8_t* ptr) noexcept
    {
        return reinterpret_cast<std::uintptr_t>(ptr);
    }

    std::uint8_t*
    from_int(std::uintptr_t i) noexcept
    {
        return reinterpret_cast<std::uint8_t*>(i);
    }

    std::uint8_t*
    list_get_next(void* address) noexcept
    {
        DEBUG_ASSERT(address != nullptr);
        return from_int(get_int(address));
    }

    void
    list_set_next(void* address, std::uint8_t* ptr) noexcept
    {
        set_int(address, to_int(ptr));
    }

    interval
    list_search_array(std::uint8_t* first, std::size_t bytes_needed, std::size_t node_size) noexcept
    {
        interval i;
        i.prev = nullptr;
        i.first = first;
        // i.last/next are used as iterator for the end of the interval
        i.last = first;
        i.next = list_get_next(first);

        auto bytes_so_far = node_size;
        while (i.next) {
            if (i.last + node_size != i.next) { // not continous
                // restart at next
                i.prev = i.last;
                i.first = i.next;
                i.last = i.next;
                i.next = list_get_next(i.last);

                bytes_so_far = node_size;
            } else {
                // extend interval
                auto new_next = list_get_next(i.next);
                i.last = i.next;
                i.next = new_next;

                bytes_so_far += node_size;
                if (bytes_so_far >= bytes_needed)
                    return i;
            }
        }
        // not enough continuous space
        return {nullptr, nullptr, nullptr, nullptr};
    }
} // namespace detail

void
free_list::insert(void* mem, std::size_t size) noexcept
{
    DEBUG_ASSERT(mem != nullptr);
    DEBUG_ASSERT(size != 0);
    insert_impl(mem, size);
}

void*
free_list::allocate() noexcept
{
    DEBUG_ASSERT(!empty());
    --capacity_left_;
    ++used_;
    max_used_ = std::max(max_used_, used_);

    std::uint8_t* mem = first_;
    first_ = detail::list_get_next(first_);

    DEBUG_ASSERT(detail::is_aligned(mem, 8));
    return mem;
}

void*
free_list::allocate(std::size_t n) noexcept
{
    DEBUG_ASSERT(!empty());
    if (n <= node_size_)
        return allocate();

    detail::interval i = detail::list_search_array(first_, n, node_size_);
    if (i.first == nullptr)
        return nullptr;

    if (i.prev != nullptr)
        detail::list_set_next(i.prev, i.next); // change next from previous to first after
    else
        first_ = i.next;
    capacity_left_ -= i.size(node_size_);
    used_ += i.size(node_size_);
    max_used_ = std::max(max_used_, used_);

    return i.first;
}

void
free_list::deallocate(void* ptr) noexcept
{
    ++capacity_left_;
    --used_;

    auto* node = static_cast<std::uint8_t*>(ptr);
    detail::list_set_next(node, first_);
    first_ = node;
}

void
free_list::deallocate(void* ptr, std::size_t n) noexcept
{
    if (n <= node_size_)
        deallocate(ptr);
    else
        insert_impl(ptr, n);
}

void
free_list::insert_impl(void* mem, std::size_t size) noexcept
{
    DEBUG_ASSERT(detail::is_aligned(mem, 8));
    std::size_t const no_nodes = size / node_size_;
    DEBUG_ASSERT(no_nodes > 0);

    auto* cur = static_cast<std::uint8_t*>(mem);
    for (std::size_t i = 0u; i != no_nodes - 1; ++i) {
        DEBUG_ASSERT(detail::is_aligned(cur, 8));
        detail::list_set_next(cur, cur + node_size_);
        cur += node_size_;
    }
    detail::list_set_next(cur, first_);
    first_ = static_cast<std::uint8_t*>(mem);

    capacity_left_ += no_nodes;
}
