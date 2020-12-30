#include "afk/io/Time.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using std::string;
using std::stringstream;

namespace afk {
  namespace io {
    auto get_date_time() -> string {
      auto ss = stringstream{};

      auto time       = std::time(nullptr);
      auto local_time = *std::localtime(&time);

      ss << std::put_time(&local_time, "[%Y-%m-%d %H:%M:%S] ");

      return ss.str();
    }
  }
}
