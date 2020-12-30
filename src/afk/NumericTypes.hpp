#pragma once

#include <climits>
#include <cstdint>
#include <cstdlib>

static_assert(sizeof(float) * CHAR_BIT == 32,
              "float is not 32 bit on this architecture");
static_assert(sizeof(double) * CHAR_BIT == 64,
              "double is not 64 bit on this architecture");

namespace afk {
  using i8    = std::int8_t;
  using i16   = std::int16_t;
  using i32   = std::int32_t;
  using i64   = std::int64_t;
  using u8    = std::uint8_t;
  using u16   = std::uint16_t;
  using u32   = std::uint32_t;
  using u64   = std::uint64_t;
  using usize = std::size_t;
  using f32   = float;
  using f64   = double;
}

using afk::f32;
using afk::f64;
using afk::i16;
using afk::i32;
using afk::i64;
using afk::i8;
using afk::u16;
using afk::u32;
using afk::u64;
using afk::u8;
using afk::usize;
