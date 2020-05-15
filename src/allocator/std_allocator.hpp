#pragma once

#include "reference_storage.hpp"
#include <type_traits> // std::true_type
#include <utility> // std::forward


template <typename T, class Allocator>
class std_allocator : reference_storage
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_swap = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::true_type;

    template <typename U>
    struct rebind
    {
        using other = std_allocator<U, Allocator>;
    };

    using allocator_type = typename reference_storage::allocator_type;

    /// ctor used when initializing std containers
    template <class Alloc>
    std_allocator(Alloc& alloc, decltype(reference_storage(alloc), int()) = 0) noexcept
            : reference_storage(alloc)
    {}

    std_allocator<value_type, Allocator>
    select_on_container_copy_construction() const
    {
        return *this;
    }

    value_type*
    allocate(size_type n, void* = nullptr)
    {
        return static_cast<value_type*>(allocate_impl(n));
    }

    void
    deallocate(value_type* p, size_type n) noexcept
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
    get_allocator() noexcept -> decltype(std::declval<reference_storage>().get_allocator())
    {
        return reference_storage::get_allocator();
    }

    auto
    get_allocator() const noexcept
            -> decltype(std::declval<reference_storage const>().get_allocator())
    {
        return reference_storage::get_allocator();
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
    equal_to_impl(std_allocator<U, Allocator> const& other) const noexcept
    {
        return &get_allocator() == &other.get_allocator();
    }

    template <typename T1, typename T2, class Impl>
    friend bool operator==(
            std_allocator<T1, Impl> const& lhs, std_allocator<T2, Impl> const& rhs) noexcept;

    template <typename U, class OtherAllocator>
    friend class std_allocator;
};


template <typename T, typename U, class Impl>
bool
operator==(std_allocator<T, Impl> const& lhs, std_allocator<U, Impl> const& rhs) noexcept
{
    return lhs.equal_to_impl(rhs);
}

template <typename T, typename U, class Impl>
bool
operator!=(std_allocator<T, Impl> const& lhs, std_allocator<U, Impl> const& rhs) noexcept
{
    return !(lhs == rhs);
}
