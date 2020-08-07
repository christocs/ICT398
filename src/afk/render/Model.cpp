#include "afk/render/Model.hpp"

#include <filesystem>

#include "afk/io/ModelLoader.hpp"

using afk::io::ModelLoader;
using afk::render::Model;

using std::filesystem::path;

Model::Model(const path &_file_path) {
  auto tmp = ModelLoader{}.load(_file_path);

  this->meshes    = std::move(tmp.meshes);
  this->file_path = std::move(tmp.file_path);
  this->file_dir  = std::move(tmp.file_dir);
}
