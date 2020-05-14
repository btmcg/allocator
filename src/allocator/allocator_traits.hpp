#pragma once

#include "common/assert.hpp"
#include <cstddef> // std::max_align_t
#include <type_traits>


namespace traits_detail {
    // full_concept has the best conversion rank, error the lowest
    // used to give priority to the functions
    struct error
    {
        operator void*() const noexcept
        {
            DEBUG_ASSERT(false);
            return nullptr;
        }
    };
    struct std_concept : error
    {};
    struct min_concept : std_concept
    {};
    struct full_concept : min_concept
    {};

    // used to delay assert in handle_error() until instantiation
    template <typename T>
    struct invalid_allocator_concept
    {
        static const bool error = false;
    };

    //=== allocator_type ===//
    // if Allocator has a member template `rebind`, use that to rebind to `char`
    // else if Allocator has a member `value_type`, rebind by changing argument
    // else does nothing
    template <class Allocator>
    typename Allocator::template rebind<char>::other& rebind_impl(int);

    template <class Allocator, typename T>
    struct allocator_rebinder
    {
        using type = Allocator&;
    };

    template <template <typename, typename...> class Alloc, typename U, typename... Args,
            typename T>
    struct allocator_rebinder<Alloc<U, Args...>, T>
    {
        using type = Alloc<T, Args...>&;
    };

    template <class Allocator, typename = typename Allocator::value_type>
    typename allocator_rebinder<Allocator, char>::type rebind_impl(char);

    template <class Allocator>
    Allocator& rebind_impl(...);

    template <class Allocator>
    using allocator_type = typename std::decay<Allocator>::type;

    //=== is_stateful ===//
    // first try to access Allocator::is_stateful,
    // then use whether or not the type is empty
    template <class Allocator>
    decltype(typename Allocator::is_stateful{}) is_stateful(full_concept);

    template <class Allocator, bool IsEmpty>
    struct is_stateful_impl;

    template <class Allocator>
    struct is_stateful_impl<Allocator, true>
    {
        static_assert(std::is_default_constructible<Allocator>::value,
                "Allocator is empty but not default constructible. This means it is not a stateless allocator. If this is actually intended provide the appropriate is_stateful typedef in your class.");
        using type = std::false_type;
    };

    template <class Allocator>
    struct is_stateful_impl<Allocator, false>
    {
        using type = std::true_type;
    };

    template <class Allocator>
    auto is_stateful(min_concept) ->
            typename is_stateful_impl<Allocator, std::is_empty<Allocator>::value>::type;

    template <class Allocator>
    auto
    allocate_node(full_concept, Allocator& alloc, std::size_t size, std::size_t alignment)
            -> decltype(alloc.allocate_node(size, alignment))
    {
        static_assert(std::is_same_v<decltype(alloc.allocate_node(size, alignment)), void*>,
                "alloc.allocate_node(size, alignment) does not have the return type void*");
        return alloc.allocate_node(size, alignment);
    }

    template <class Allocator>
    auto
    allocate_node(std_concept, Allocator& alloc, std::size_t size, std::size_t)
            -> decltype(static_cast<void*>(alloc.allocate(size)))
    {
        return static_cast<void*>(alloc.allocate(size));
    }

    template <class Allocator>
    error
    allocate_node(error, Allocator&, std::size_t, std::size_t)
    {
        static_assert(invalid_allocator_concept<Allocator>::error,
                "type is not a Allocator as it does not provide: void* allocate_node(std::size_t, std::size_t)");
        return {};
    }

    template <class Allocator>
    auto
    deallocate_node(full_concept, Allocator& alloc, void* ptr, std::size_t size,
            std::size_t alignment) noexcept -> decltype(alloc.deallocate_node(ptr, size, alignment))
    {
        static_assert(std::is_same_v<decltype(alloc.deallocate_node(ptr, size, alignment)), void>,
                "alloc.deallocate_node(ptr, size, alignment) does not have the return type void");
        return alloc.deallocate_node(ptr, size, alignment);
    }

    template <class Allocator>
    auto
    deallocate_node(std_concept, Allocator& alloc, void* ptr, std::size_t size,
            std::size_t) noexcept -> decltype(alloc.deallocate(static_cast<char*>(ptr), size))
    {
        static_assert(
                std::is_same_v<decltype(alloc.deallocate(static_cast<char*>(ptr), size)), void>,
                "alloc.deallocate(static_cast<char*>(ptr), size) does not have the return type void");
        return alloc.deallocate(static_cast<char*>(ptr), size);
    }

    template <class Allocator>
    error
    deallocate_node(error, Allocator&, void*, std::size_t, std::size_t)
    {
        static_assert(invalid_allocator_concept<Allocator>::error,
                "type is not a Allocator as it does not provide: void deallocate_node(void*, std::size_t, std::size_t)");
        return error{};
    }

    template <class Allocator>
    auto
    allocate_array(full_concept, Allocator& alloc, std::size_t count, std::size_t size,
            std::size_t alignment) -> decltype(alloc.allocate_array(count, size, alignment))
    {
        static_assert(std::is_same_v<decltype(alloc.allocate_array(count, size, alignment)), void*>,
                "alloc.allocate_array(count, size, alignment) does not have the return type void*");
        return alloc.allocate_array(count, size, alignment);
    }

    template <class Allocator>
    void*
    allocate_array(min_concept, Allocator& alloc, std::size_t count, std::size_t size,
            std::size_t alignment)
    {
        return allocate_node(full_concept{}, alloc, count * size, alignment);
    }

    template <class Allocator>
    auto
    deallocate_array(full_concept, Allocator& alloc, void* ptr, std::size_t count, std::size_t size,
            std::size_t alignment) noexcept
            -> decltype(alloc.deallocate_array(ptr, count, size, alignment))
    {
        static_assert(
                std::is_same_v<decltype(alloc.deallocate_array(ptr, count, size, alignment)), void>,
                "alloc.deallocate_array(ptr, count, size, alignment) does not have the return type void");
        return alloc.deallocate_array(ptr, count, size, alignment);
    }

    template <class Allocator>
    void
    deallocate_array(min_concept, Allocator& alloc, void* ptr, std::size_t count, std::size_t size,
            std::size_t alignment) noexcept
    {
        deallocate_node(full_concept{}, alloc, ptr, count * size, alignment);
    }

    template <class Allocator>
    auto
    max_node_size(full_concept, const Allocator& alloc) -> decltype(alloc.max_node_size())
    {
        static_assert(std::is_same_v<decltype(alloc.max_node_size()), std::size_t>,
                "alloc.max_node_size() does not have the return type std::size_t");
        return alloc.max_node_size();
    }

    template <class Allocator>
    std::size_t
    max_node_size(min_concept, const Allocator&) noexcept
    {
        return std::size_t(-1);
    }

    template <class Allocator>
    auto
    max_array_size(full_concept, const Allocator& alloc) -> decltype(alloc.max_array_size())
    {
        static_assert(std::is_same_v<decltype(alloc.max_array_size()), std::size_t>,
                "alloc.max_array_size() does not have the return type std::size_t");
        return alloc.max_array_size();
    }

    template <class Allocator>
    std::size_t
    max_array_size(min_concept, const Allocator& alloc)
    {
        return max_node_size(full_concept{}, alloc);
    }

    template <class Allocator>
    auto
    max_alignment(full_concept, const Allocator& alloc) -> decltype(alloc.max_alignment())
    {
        static_assert(std::is_same_v<decltype(alloc.max_alignment()), std::size_t>,
                "alloc.max_alignment() does not have the return type std::size_t");
        return alloc.max_alignment();
    }

    template <class Allocator>
    std::size_t
    max_alignment(min_concept, const Allocator&)
    {
        return alignof(std::max_align_t);
    }

} // namespace traits_detail


template <class Allocator>
class allocator_traits
{
public:
    using allocator_type = traits_detail::allocator_type<Allocator>;
    using is_stateful
            = decltype(traits_detail::is_stateful<Allocator>(traits_detail::full_concept{}));

    static void*
    allocate_node(allocator_type& state, std::size_t size, std::size_t alignment)
    {
        return traits_detail::allocate_node(traits_detail::full_concept{}, state, size, alignment);
    }

    static void*
    allocate_array(
            allocator_type& state, std::size_t count, std::size_t size, std::size_t alignment)
    {
        return traits_detail::allocate_array(
                traits_detail::full_concept{}, state, count, size, alignment);
    }

    static void
    deallocate_node(
            allocator_type& state, void* node, std::size_t size, std::size_t alignment) noexcept
    {
        traits_detail::deallocate_node(traits_detail::full_concept{}, state, node, size, alignment);
    }

    static void
    deallocate_array(allocator_type& state, void* array, std::size_t count, std::size_t size,
            std::size_t alignment) noexcept
    {
        traits_detail::deallocate_array(
                traits_detail::full_concept{}, state, array, count, size, alignment);
    }

    static std::size_t
    max_node_size(const allocator_type& state)
    {
        return traits_detail::max_node_size(traits_detail::full_concept{}, state);
    }

    static std::size_t
    max_array_size(const allocator_type& state)
    {
        return traits_detail::max_array_size(traits_detail::full_concept{}, state);
    }

    static std::size_t
    max_alignment(const allocator_type& state)
    {
        return traits_detail::max_alignment(traits_detail::full_concept{}, state);
    }
};
