#include "afk/io/Json.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include "afk/debug/Assert.hpp"

using std::ifstream;
using std::ofstream;
using std::filesystem::path;
using namespace std::string_literals;

namespace afk {
  namespace io {
    auto read_json_from_file(const path &file_path) -> Json {
      auto file = ifstream{file_path};
      auto json = Json{};

      afk_assert(file.is_open(), "Unable to open JSON file "s + file_path.string());

      json << file;

      return json;
    }

    auto write_json_to_file(const path &file_path, const Json &json) -> void {
      auto file = ofstream{file_path};
      file << json.dump(JSON_INDENTING) << '\n';
    }
  }
}
