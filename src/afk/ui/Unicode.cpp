#include "afk/ui/Unicode.hpp"

using std::ostream;
using std::u8string;

ostream &operator<<(ostream &os, const char8_t *c) {
  return os << reinterpret_cast<const char *>(c);
}

ostream &operator<<(ostream &os, const u8string &s) {
  return os << reinterpret_cast<const char *>(s.data());
}

auto afk::ui::to_cstr(const std::u8string &s) -> const char * {
  return reinterpret_cast<const char *>(s.data());
}
