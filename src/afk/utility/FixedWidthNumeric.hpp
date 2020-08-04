#include <climits>
#include <cstdint>

static_assert(sizeof(float) * CHAR_BIT == 32,
              "float is not 32 bit on this architecture");
static_assert(sizeof(double) * CHAR_BIT == 64,
              "float is not 32 bit on this architecture");

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using usize = std::size_t;
