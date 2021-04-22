#pragma once

#include <cstddef> // std::size_t
#include <cstdint>


namespace detail {
    inline constexpr std::int32_t DefaultAlignment = 8;
    inline constexpr std::size_t MinElementSize = sizeof(std::uint8_t*);

    /// returns next greater multiple of 8 to ensure 8-byte alignment
    constexpr std::size_t round_up_to_align(
            std::size_t& x, std::int32_t alignment = DefaultAlignment);

    /// whether or not an alignment is valid, i.e. a power of two not zero
    constexpr bool is_valid_alignment(std::size_t alignment) noexcept;

    bool is_aligned(void* ptr, std::size_t alignment) noexcept;

    /**********************************************************************/

    constexpr std::size_t
    round_up_to_align(std::size_t& x, std::int32_t alignment)
    {
        return ((x + (alignment - 1)) & (-alignment));
    }

    /// whether or not an alignment is valid, i.e. a power of two not zero
    constexpr bool
    is_valid_alignment(std::size_t alignment) noexcept
    {
        return alignment && (alignment & (alignment - 1)) == 0u;
    }
} // namespace detail
