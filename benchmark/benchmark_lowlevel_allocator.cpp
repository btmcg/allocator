#include "allocator/lowlevel_allocator.hpp"
#include <benchmark/benchmark.h>
#include <cstdint>
#include <limits>
#include <random>
#include <unordered_set>

namespace { // unnamed

    constexpr int DistMin = 1;
    constexpr int DistMax = 1024 * 1024;

    constexpr inline std::size_t
    make_multiple_of_8(std::size_t x)
    {
        constexpr std::size_t digits = std::numeric_limits<std::uint8_t>::digits - 1;
        return (x + digits) & ~(digits);
    }

} // namespace

static void
malloc_allocator_bm(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(DistMin, DistMax);

    lowlevel_allocator<malloc_allocator> alloc;
    std::unordered_set<int*> set;
    constexpr std::size_t alignment = 8;

    for (auto _ : state) // NOLINT
        set.emplace(
                static_cast<int*>(alloc.allocate_node(make_multiple_of_8(dist(rng)), alignment)));

    for (auto& itr : set)
        alloc.deallocate_node(itr, 0, alignment);
}
BENCHMARK(malloc_allocator_bm);

static void
new_allocator_bm(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(DistMin, DistMax);

    lowlevel_allocator<new_allocator> alloc;
    std::unordered_set<int*> set;
    constexpr std::size_t alignment = 8;

    for (auto _ : state) // NOLINT
        set.emplace(
                static_cast<int*>(alloc.allocate_node(make_multiple_of_8(dist(rng)), alignment)));

    for (auto& itr : set)
        alloc.deallocate_node(itr, 0, alignment);
}
BENCHMARK(new_allocator_bm);

static void
posix_allocator_bm(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(DistMin, DistMax);

    lowlevel_allocator<posix_allocator> alloc;
    std::unordered_set<int*> set;
    constexpr std::size_t alignment = 8;

    for (auto _ : state) // NOLINT
        set.emplace(
                static_cast<int*>(alloc.allocate_node(make_multiple_of_8(dist(rng)), alignment)));

    for (auto& itr : set)
        alloc.deallocate_node(itr, 0, alignment);
}
BENCHMARK(posix_allocator_bm);
