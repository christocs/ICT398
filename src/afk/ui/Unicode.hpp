#pragma once

#include <array>
#include <iostream>
#include <string>
#include <utility>

#include <imgui/imgui.h>

#include "afk/utility/ArrayOf.hpp"

std::ostream &operator<<(std::ostream &os, const char8_t *c8);
std::ostream &operator<<(std::ostream &os, const std::u8string &c8);

namespace afk {
  namespace ui {
    auto to_cstr(const std::u8string &s) -> const char *;

    /**
     * List of the Unicode character ranges.
     */
    // clang-format off
    constexpr auto unicode_ranges = utility::array_of<ImWchar>(
      static_cast<ImWchar>(0x0020), static_cast<ImWchar>(0x007F), // Basic Latin
      static_cast<ImWchar>(0x00A0), static_cast<ImWchar>(0x00FF), // Latin-1 Supplement
      static_cast<ImWchar>(0x0100), static_cast<ImWchar>(0x017F), // Latin Extended-A
      static_cast<ImWchar>(0x0180), static_cast<ImWchar>(0x024F), // Latin Extended-B
      static_cast<ImWchar>(0x0250), static_cast<ImWchar>(0x02AF), // IPA Extensions
      static_cast<ImWchar>(0x02B0), static_cast<ImWchar>(0x02FF), // Spacing Modifier Letters
      static_cast<ImWchar>(0x0300), static_cast<ImWchar>(0x036F), // Combining Diacritical Marks
      static_cast<ImWchar>(0x0370), static_cast<ImWchar>(0x03FF), // Greek and Coptic
      static_cast<ImWchar>(0x0400), static_cast<ImWchar>(0x04FF), // Cyrillic
      static_cast<ImWchar>(0x0500), static_cast<ImWchar>(0x052F), // Cyrillic Supplementary
      static_cast<ImWchar>(0x0530), static_cast<ImWchar>(0x058F), // Armenian
      static_cast<ImWchar>(0x0590), static_cast<ImWchar>(0x05FF), // Hebrew
      static_cast<ImWchar>(0x0600), static_cast<ImWchar>(0x06FF), // Arabic
      static_cast<ImWchar>(0x0700), static_cast<ImWchar>(0x074F), // Syriac
      static_cast<ImWchar>(0x0780), static_cast<ImWchar>(0x07BF), // Thaana
      static_cast<ImWchar>(0x0900), static_cast<ImWchar>(0x097F), // Devanagari
      static_cast<ImWchar>(0x0980), static_cast<ImWchar>(0x09FF), // Bengali
      static_cast<ImWchar>(0x0A00), static_cast<ImWchar>(0x0A7F), // Gurmukhi
      static_cast<ImWchar>(0x0A80), static_cast<ImWchar>(0x0AFF), // Gujarati
      static_cast<ImWchar>(0x0B00), static_cast<ImWchar>(0x0B7F), // Oriya
      static_cast<ImWchar>(0x0B80), static_cast<ImWchar>(0x0BFF), // Tamil
      static_cast<ImWchar>(0x0C00), static_cast<ImWchar>(0x0C7F), // Telugu
      static_cast<ImWchar>(0x0C80), static_cast<ImWchar>(0x0CFF), // Kannada
      static_cast<ImWchar>(0x0D00), static_cast<ImWchar>(0x0D7F), // Malayalam
      static_cast<ImWchar>(0x0D80), static_cast<ImWchar>(0x0DFF), // Sinhala
      static_cast<ImWchar>(0x0E00), static_cast<ImWchar>(0x0E7F), // Thai
      static_cast<ImWchar>(0x0E80), static_cast<ImWchar>(0x0EFF), // Lao
      static_cast<ImWchar>(0x0F00), static_cast<ImWchar>(0x0FFF), // Tibetan
      static_cast<ImWchar>(0x1000), static_cast<ImWchar>(0x109F), // Myanmar
      static_cast<ImWchar>(0x10A0), static_cast<ImWchar>(0x10FF), // Georgian
      static_cast<ImWchar>(0x1100), static_cast<ImWchar>(0x11FF), // Hangul Jamo
      static_cast<ImWchar>(0x1200), static_cast<ImWchar>(0x137F), // Ethiopic
      static_cast<ImWchar>(0x13A0), static_cast<ImWchar>(0x13FF), // Cherokee
      static_cast<ImWchar>(0x1400), static_cast<ImWchar>(0x167F), // Unified Canadian Aboriginal Syllabics
      static_cast<ImWchar>(0x1680), static_cast<ImWchar>(0x169F), // Ogham
      static_cast<ImWchar>(0x16A0), static_cast<ImWchar>(0x16FF), // Runic
      static_cast<ImWchar>(0x1700), static_cast<ImWchar>(0x171F), // Tagalog
      static_cast<ImWchar>(0x1720), static_cast<ImWchar>(0x173F), // Hanunoo
      static_cast<ImWchar>(0x1740), static_cast<ImWchar>(0x175F), // Buhid
      static_cast<ImWchar>(0x1760), static_cast<ImWchar>(0x177F), // Tagbanwa
      static_cast<ImWchar>(0x1780), static_cast<ImWchar>(0x17FF), // Khmer
      static_cast<ImWchar>(0x1800), static_cast<ImWchar>(0x18AF), // Mongolian
      static_cast<ImWchar>(0x1900), static_cast<ImWchar>(0x194F), // Limbu
      static_cast<ImWchar>(0x1950), static_cast<ImWchar>(0x197F), // Tai Le
      static_cast<ImWchar>(0x19E0), static_cast<ImWchar>(0x19FF), // Khmer Symbols
      static_cast<ImWchar>(0x1D00), static_cast<ImWchar>(0x1D7F), // Phonetic Extensions
      static_cast<ImWchar>(0x1E00), static_cast<ImWchar>(0x1EFF), // Latin Extended Additional
      static_cast<ImWchar>(0x1F00), static_cast<ImWchar>(0x1FFF), // Greek Extended
      static_cast<ImWchar>(0x2000), static_cast<ImWchar>(0x206F), // General Punctuation
      static_cast<ImWchar>(0x2070), static_cast<ImWchar>(0x209F), // Superscripts and Subscripts
      static_cast<ImWchar>(0x20A0), static_cast<ImWchar>(0x20CF), // Currency Symbols
      static_cast<ImWchar>(0x20D0), static_cast<ImWchar>(0x20FF), // Combining Diacritical Marks for Symbols
      static_cast<ImWchar>(0x2100), static_cast<ImWchar>(0x214F), // Letterlike Symbols
      static_cast<ImWchar>(0x2150), static_cast<ImWchar>(0x218F), // Number Forms
      static_cast<ImWchar>(0x2190), static_cast<ImWchar>(0x21FF), // Arrows
      static_cast<ImWchar>(0x2200), static_cast<ImWchar>(0x22FF), // Mathematical Operators
      static_cast<ImWchar>(0x2300), static_cast<ImWchar>(0x23FF), // Miscellaneous Technical
      static_cast<ImWchar>(0x2400), static_cast<ImWchar>(0x243F), // Control Pictures
      static_cast<ImWchar>(0x2440), static_cast<ImWchar>(0x245F), // Optical Character Recognition
      static_cast<ImWchar>(0x2460), static_cast<ImWchar>(0x24FF), // Enclosed Alphanumerics
      static_cast<ImWchar>(0x2500), static_cast<ImWchar>(0x257F), // Box Drawing
      static_cast<ImWchar>(0x2580), static_cast<ImWchar>(0x259F), // Block Elements
      static_cast<ImWchar>(0x25A0), static_cast<ImWchar>(0x25FF), // Geometric Shapes
      static_cast<ImWchar>(0x2600), static_cast<ImWchar>(0x26FF), // Miscellaneous Symbols
      static_cast<ImWchar>(0x2700), static_cast<ImWchar>(0x27BF), // Dingbats
      static_cast<ImWchar>(0x27C0), static_cast<ImWchar>(0x27EF), // Miscellaneous Mathematical Symbols-A
      static_cast<ImWchar>(0x27F0), static_cast<ImWchar>(0x27FF), // Supplemental Arrows-A
      static_cast<ImWchar>(0x2800), static_cast<ImWchar>(0x28FF), // Braille Patterns
      static_cast<ImWchar>(0x2900), static_cast<ImWchar>(0x297F), // Supplemental Arrows-B
      static_cast<ImWchar>(0x2980), static_cast<ImWchar>(0x29FF), // Miscellaneous Mathematical Symbols-B
      static_cast<ImWchar>(0x2A00), static_cast<ImWchar>(0x2AFF), // Supplemental Mathematical Operators
      static_cast<ImWchar>(0x2B00), static_cast<ImWchar>(0x2BFF), // Miscellaneous Symbols and Arrows
      static_cast<ImWchar>(0x2E80), static_cast<ImWchar>(0x2EFF), // CJK Radicals Supplement
      static_cast<ImWchar>(0x2F00), static_cast<ImWchar>(0x2FDF), // Kangxi Radicals
      static_cast<ImWchar>(0x2FF0), static_cast<ImWchar>(0x2FFF), // Ideographic Description Characters
      static_cast<ImWchar>(0x3000), static_cast<ImWchar>(0x303F), // CJK Symbols and Punctuation
      static_cast<ImWchar>(0x3040), static_cast<ImWchar>(0x309F), // Hiragana
      static_cast<ImWchar>(0x30A0), static_cast<ImWchar>(0x30FF), // Katakana
      static_cast<ImWchar>(0x3100), static_cast<ImWchar>(0x312F), // Bopomofo
      static_cast<ImWchar>(0x3130), static_cast<ImWchar>(0x318F), // Hangul Compatibility Jamo
      static_cast<ImWchar>(0x3190), static_cast<ImWchar>(0x319F), // Kanbun
      static_cast<ImWchar>(0x31A0), static_cast<ImWchar>(0x31BF), // Bopomofo Extended
      static_cast<ImWchar>(0x31F0), static_cast<ImWchar>(0x31FF), // Katakana Phonetic Extensions
      static_cast<ImWchar>(0x3200), static_cast<ImWchar>(0x32FF), // Enclosed CJK Letters and Months
      static_cast<ImWchar>(0x3300), static_cast<ImWchar>(0x33FF), // CJK Compatibility
      static_cast<ImWchar>(0x3400), static_cast<ImWchar>(0x4DBF), // CJK Unified Ideographs Extension A
      static_cast<ImWchar>(0x4DC0), static_cast<ImWchar>(0x4DFF), // Yijing Hexagram Symbols
      static_cast<ImWchar>(0x4E00), static_cast<ImWchar>(0x9FFF), // CJK Unified Ideographs
      static_cast<ImWchar>(0xA000), static_cast<ImWchar>(0xA48F), // Yi Syllables
      static_cast<ImWchar>(0xA490), static_cast<ImWchar>(0xA4CF), // Yi Radicals
      static_cast<ImWchar>(0xAC00), static_cast<ImWchar>(0xD7AF), // Hangul Syllables
      static_cast<ImWchar>(0xD800), static_cast<ImWchar>(0xDB7F), // High Surrogates
      static_cast<ImWchar>(0xDB80), static_cast<ImWchar>(0xDBFF), // High Private Use Surrogates
      static_cast<ImWchar>(0xDC00), static_cast<ImWchar>(0xDFFF), // Low Surrogates
      static_cast<ImWchar>(0xE000), static_cast<ImWchar>(0xF8FF), // Private Use Area
      static_cast<ImWchar>(0xF900), static_cast<ImWchar>(0xFAFF), // CJK Compatibility Ideographs
      static_cast<ImWchar>(0xFB00), static_cast<ImWchar>(0xFB4F), // Alphabetic Presentation Forms
      static_cast<ImWchar>(0xFB50), static_cast<ImWchar>(0xFDFF), // Arabic Presentation Forms-A
      static_cast<ImWchar>(0xFE00), static_cast<ImWchar>(0xFE0F), // Variation Selectors
      static_cast<ImWchar>(0xFE20), static_cast<ImWchar>(0xFE2F), // Combining Half Marks
      static_cast<ImWchar>(0xFE30), static_cast<ImWchar>(0xFE4F), // CJK Compatibility Forms
      static_cast<ImWchar>(0xFE50), static_cast<ImWchar>(0xFE6F), // Small Form Variants
      static_cast<ImWchar>(0xFE70), static_cast<ImWchar>(0xFEFF), // Arabic Presentation Forms-B
      static_cast<ImWchar>(0xFF00), static_cast<ImWchar>(0xFFEF), // Halfwidth and Fullwidth Forms
      static_cast<ImWchar>(0xFFF0), static_cast<ImWchar>(0xFFFF), // Specials
      static_cast<ImWchar>(0)
    );
  }
  // clang-format on
};
