#ifndef POKERSTOVE_UTIL_LASTBIT_H
#define POKERSTOVE_UTIL_LASTBIT_H

#include <bit>
#include <cstdint>
#include <pokerstove/util/utypes.h>

// Preserve the legacy convention: an empty mask has bit index zero.
inline constexpr uint firstbit(uint64_t v) noexcept
{
    return v == 0 ? 0 : static_cast<uint>(std::bit_width(v) - 1);
}

inline constexpr int lastbit(uint32_t v) noexcept
{
    return v == 0 ? 0 : std::countr_zero(v);
}

inline constexpr int lastbit(uint16_t v) noexcept
{
    return v == 0 ? 0 : std::countr_zero(v);
}

inline constexpr int lastbit(uint64_t v) noexcept
{
    return v == 0 ? 0 : std::countr_zero(v);
}

inline constexpr int lastbit64(uint64_t v) noexcept
{
    return lastbit(v);
}

#endif  // POKERSTOVE_UTIL_LASTBIT_H
