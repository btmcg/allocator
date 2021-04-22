#include "detail.hpp"
#include "util/assert.hpp"


bool
is_aligned(void* ptr, std::size_t alignment) noexcept
{
    DEBUG_ASSERT(is_valid_alignment(alignment));
    auto address = reinterpret_cast<std::uintptr_t>(ptr);
    return address % alignment == 0u;
}
