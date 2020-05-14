#pragma once

#include "allocator_storage.hpp"
#include "memory_pool.hpp"
#include "reference_storage.hpp"
#include <type_traits> // std::true_type
#include <utility> // std::forward


template <typename T, class RawAllocator>
class std_allocator : reference_storage
{
    using alloc_reference = reference_storage;

public:
    //=== typedefs ===//
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using propagate_on_container_swap = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::true_type;

    template <typename U>
    struct rebind
    {
        using other = std_allocator<U, RawAllocator>;
    };

    using allocator_type = typename alloc_reference::allocator_type;

    // std_allocator() noexcept
    //         : alloc_reference(allocator_type{})
    // {}

    template <class RawAlloc>
    std_allocator(RawAlloc& alloc, decltype(alloc_reference(alloc), int()) = 0) noexcept
            : alloc_reference(alloc)
    {}

    template <class RawAlloc>
    std_allocator(const RawAlloc& alloc, decltype(alloc_reference(alloc))) noexcept
            : alloc_reference(alloc)
    {}

    std_allocator(const alloc_reference& alloc) noexcept
            : alloc_reference(alloc)
    {}

    template <class StoragePolicy>
    std_allocator(const allocator_storage<StoragePolicy>&) = delete;

    template <typename U>
    std_allocator(const std_allocator<U, RawAllocator>& alloc) noexcept
            : alloc_reference(alloc)
    {}

    template <typename U>
    std_allocator(std_allocator<U, RawAllocator>& alloc) noexcept
            : alloc_reference(alloc)
    {}

    std_allocator<T, RawAllocator>
    select_on_container_copy_construction() const
    {
        return *this;
    }

    pointer
    allocate(size_type n, void* = nullptr)
    {
        return static_cast<pointer>(allocate_impl(n));
    }

    void
    deallocate(pointer p, size_type n) noexcept
    {
        deallocate_impl(p, n);
    }

    template <typename U, typename... Args>
    void
    construct(U* p, Args&&... args)
    {
        void* mem = p;
        ::new (mem) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void
    destroy(U* p) noexcept
    {
        p->~U();
    }

    size_type
    max_size() const noexcept
    {
        return this->max_array_size() / sizeof(value_type);
    }

    auto
    get_allocator() noexcept -> decltype(std::declval<alloc_reference>().get_allocator())
    {
        return alloc_reference::get_allocator();
    }

    auto
    get_allocator() const noexcept
            -> decltype(std::declval<const alloc_reference>().get_allocator())
    {
        return alloc_reference::get_allocator();
    }

private:
    void*
    allocate_impl(size_type n)
    {
        if (n == 1)
            return get_allocator().allocate_node();
        else
            return get_allocator().allocate_array(n);
    }

    void
    deallocate_impl(void* ptr, size_type n)
    {
        if (n == 1)
            get_allocator().deallocate_node(ptr);
        else
            get_allocator().deallocate_array(ptr, n);
    }

    template <typename U> // stateful
    bool
    equal_to_impl(const std_allocator<U, RawAllocator>& other) const noexcept
    {
        return &get_allocator() == &other.get_allocator();
    }

    template <typename T1, typename T2, class Impl>
    friend bool operator==(
            const std_allocator<T1, Impl>& lhs, const std_allocator<T2, Impl>& rhs) noexcept;

    template <typename U, class OtherRawAllocator>
    friend class std_allocator;
};


template <typename T, typename U, class Impl>
bool
operator==(const std_allocator<T, Impl>& lhs, const std_allocator<U, Impl>& rhs) noexcept
{
    return lhs.equal_to_impl(rhs);
}

template <typename T, typename U, class Impl>
bool
operator!=(const std_allocator<T, Impl>& lhs, const std_allocator<U, Impl>& rhs) noexcept
{
    return !(lhs == rhs);
}
