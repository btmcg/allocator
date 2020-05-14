#pragma once

#include "allocator_traits.hpp"


template <class StoragePolicy>
class allocator_storage : StoragePolicy
{
    using traits = allocator_traits<typename StoragePolicy::allocator_type>;

public:
    using allocator_type = typename StoragePolicy::allocator_type;
    using storage_policy = StoragePolicy;
    using is_stateful = typename traits::is_stateful;

    allocator_storage() = default;

    template <class Alloc,
            typename std::enable_if<
                    (!std::is_base_of<allocator_storage, typename std::decay<Alloc>::type>::value),
                    int>::type
            = 0>
    allocator_storage(
            Alloc&& alloc, decltype(new storage_policy(forward<Alloc>(alloc))), int() = nullptr)
            : storage_policy(forward<Alloc>(alloc))
    {}

    template <class OtherPolicy>
    allocator_storage(const allocator_storage<OtherPolicy>& other,
            decltype(new storage_policy(other.get_allocator())), int() = nullptr)
            : storage_policy(other.get_allocator())
    {}

    allocator_storage(allocator_storage&& other) noexcept
            : storage_policy(move(other))
    {}

    allocator_storage&
    operator=(allocator_storage&& other) noexcept
    {
        storage_policy::operator=(move(other));
        return *this;
    }

    allocator_storage(const allocator_storage&) = default;
    allocator_storage& operator=(const allocator_storage&) = default;

    void*
    allocate_node(std::size_t size, std::size_t alignment)
    {
        auto&& alloc = get_allocator();
        return traits::allocate_node(alloc, size, alignment);
    }

    void*
    allocate_array(std::size_t count, std::size_t size, std::size_t alignment)
    {
        auto&& alloc = get_allocator();
        return traits::allocate_array(alloc, count, size, alignment);
    }

    void
    deallocate_node(void* ptr, std::size_t size, std::size_t alignment) noexcept
    {
        auto&& alloc = get_allocator();
        traits::deallocate_node(alloc, ptr, size, alignment);
    }

    void
    deallocate_array(void* ptr, std::size_t count, std::size_t size, std::size_t alignment) noexcept
    {
        auto&& alloc = get_allocator();
        traits::deallocate_array(alloc, ptr, count, size, alignment);
    }

    std::size_t
    max_node_size() const
    {
        auto&& alloc = get_allocator();
        return traits::max_node_size(alloc);
    }

    std::size_t
    max_array_size() const
    {
        auto&& alloc = get_allocator();
        return traits::max_array_size(alloc);
    }

    std::size_t
    max_alignment() const
    {
        auto&& alloc = get_allocator();
        return traits::max_alignment(alloc);
    }

    auto
    get_allocator() noexcept -> decltype(std::declval<storage_policy>().get_allocator())
    {
        return storage_policy::get_allocator();
    }

    auto
    get_allocator() const noexcept -> decltype(std::declval<const storage_policy>().get_allocator())
    {
        return storage_policy::get_allocator();
    }


    bool
    is_composable() const noexcept
    {
        return StoragePolicy::is_composable();
    }
};
