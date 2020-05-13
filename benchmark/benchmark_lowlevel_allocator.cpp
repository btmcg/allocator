#include "allocator/lowlevel_allocator.hpp"
#include <benchmark/benchmark.h>
#include <cstdint>
#include <random>
#include <unordered_set>


constexpr inline std::size_t
make_multiple_of_8(std::size_t x)
{
    return (x + 7) & ~(7);
}

static void
malloc_allocator_bm(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 1024 * 1024);

    lowlevel_allocator<malloc_allocator> alloc;
    std::unordered_set<int*> set;
    constexpr std::size_t alignment = 8;

    for (auto _ : state)
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
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 1024 * 1024);

    lowlevel_allocator<new_allocator> alloc;
    std::unordered_set<int*> set;
    constexpr std::size_t alignment = 8;

    for (auto _ : state)
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
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 1024 * 1024);

    lowlevel_allocator<posix_allocator> alloc;
    std::unordered_set<int*> set;
    constexpr std::size_t alignment = 8;

    for (auto _ : state)
        set.emplace(
                static_cast<int*>(alloc.allocate_node(make_multiple_of_8(dist(rng)), alignment)));

    for (auto& itr : set)
        alloc.deallocate_node(itr, 0, alignment);
}
BENCHMARK(posix_allocator_bm);
