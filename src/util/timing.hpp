#pragma once

#include <chrono>

namespace pokerstove
{
// Elapsed wall seconds since a monotonic clock sample; internal helper.
inline double elapsed(std::chrono::steady_clock::time_point start)
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}
}  // namespace pokerstove
