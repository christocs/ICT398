#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/fwd.hpp>
// Must be included after GLAD.
#include <GLFW/glfw3.h>

#include "afk/NumericTypes.hpp"
#include "afk/render/Animation.hpp"
#include "afk/render/Model.hpp"
#include "afk/render/Shader.hpp"
#include "afk/render/opengl/MeshHandle.hpp"
#include "afk/render/opengl/ModelHandle.hpp"
#include "afk/render/opengl/ShaderHandle.hpp"
#include "afk/render/opengl/ShaderProgramHandle.hpp"
#include "afk/render/opengl/TextureHandle.hpp"
#include "afk/NumericTypes.hpp"

namespace afk {
  namespace render {
    struct Model;
    struct Mesh;
    struct Model;
    struct Texture;
    struct ShaderProgram;

    namespace opengl {
      class renderer {
      public:
        using MeshHandle          = opengl::MeshHandle;
        using ModelHandle         = opengl::ModelHandle;
        using ShaderHandle        = opengl::ShaderHandle;
        using ShaderProgramHandle = opengl::ShaderProgramHandle;
        using TextureHandle       = opengl::TextureHandle;

        struct PathHash {
          auto operator()(const std::filesystem::path &p) const -> usize {
            return std::filesystem::hash_value(p);
          }
        };

        struct PathEquals {
          auto operator()(const std::filesystem::path &lhs,
                          const std::filesystem::path &rhs) const -> bool {
            return lhs.lexically_normal() == rhs.lexically_normal();
          }
        };

        struct DrawCommand {
          const std::filesystem::path model_path          = {};
          const std::filesystem::path shader_program_path = {};
          const physics::Transform transform              = {};
        };

        using Models =
            std::unordered_map<std::filesystem::path, ModelHandle, PathHash, PathEquals>;
        using Textures =
            std::unordered_map<std::filesystem::path, TextureHandle, PathHash, PathEquals>;
        using Shaders =
            std::unordered_map<std::filesystem::path, ShaderHandle, PathHash, PathEquals>;
        using ShaderPrograms =
            std::unordered_map<std::filesystem::path, ShaderProgramHandle, PathHash, PathEquals>;
        using DrawQueue = std::queue<DrawCommand>;
        using Animations =
            std::unordered_map<std::filesystem::path, Model::Animations, PathHash, PathEquals>;

        using Window = std::add_pointer<GLFWwindow>::type;

        Window window = nullptr;

        renderer();
        ~renderer();
        renderer(renderer &&)      = delete;
        renderer(const renderer &) = delete;
        auto operator=(const renderer &) -> renderer & = delete;
        auto operator=(renderer &&) -> renderer & = delete;

        auto initialize() -> void;
        auto set_option(GLenum option, bool state) const -> void;
        auto check_errors() const -> void;
        auto get_window_size() const -> glm::ivec2;

        auto clear_screen(glm::vec4 clear_color = {255.0f, 255.0f, 255.0f, 1.0f}) const
            -> void;
        auto swap_buffers() -> void;
        auto set_viewport(i32 x, i32 y, i32 width, i32 height) const -> void;
        auto draw() -> void;
        auto queue_draw(DrawCommand command) -> void;
        auto draw_model(const ModelHandle &model, const ShaderProgramHandle &shader_program,
                        physics::Transform transform) const -> void;
        auto setup_view(const ShaderProgramHandle &shader_program) const -> void;

        auto use_shader(const ShaderProgramHandle &shader) const -> void;
        auto set_texture_unit(usize unit) const -> void;
        auto bind_texture(const TextureHandle &texture) const -> void;

        auto get_model(const std::filesystem::path &file_path) -> const ModelHandle &;
        auto get_texture(const std::filesystem::path &file_path) -> const TextureHandle &;
        auto get_shader(const std::filesystem::path &file_path) -> const ShaderHandle &;
        auto get_shader_program(const std::filesystem::path &file_path)
            -> const ShaderProgramHandle &;

        auto load_model(const Model &model) -> ModelHandle;
        auto load_texture(const Texture &texture) -> TextureHandle;
        auto load_mesh(const Mesh &meshData) -> MeshHandle;
        auto compile_shader(const Shader &shader) -> ShaderHandle;
        auto link_shaders(const ShaderProgram &shader_program) -> ShaderProgramHandle;

        auto set_uniform(const ShaderProgramHandle &program,
                         const std::string &name, bool value) const -> void;
        auto set_uniform(const ShaderProgramHandle &program,
                         const std::string &name, i32 value) const -> void;
        auto set_uniform(const ShaderProgramHandle &program,
                         const std::string &name, f32 value) const -> void;
        auto set_uniform(const ShaderProgramHandle &program,
                         const std::string &name, glm::vec3 value) const -> void;
        auto set_uniform(const ShaderProgramHandle &program,
                         const std::string &name, glm::mat4 value) const -> void;
        auto set_uniform(const ShaderProgramHandle &program, const std::string &name,
                         const std::vector<glm::mat4> &value) const -> void;

        auto set_wireframe(bool status) -> void;
        auto get_wireframe() const -> bool;

        auto get_models() const -> const Models &;
        auto get_textures() const -> const Textures &;
        auto get_shaders() const -> const Shaders &;
        auto get_shader_programs() const -> const ShaderPrograms &;

      private:
        static constexpr i32 opengl_major_version = 4;
        static constexpr i32 opengl_minor_version = 1;
        static constexpr bool enable_vsync        = true;

        bool is_initialized    = false;
        bool wireframe_enabled = false;

        Models models                  = {};
        Textures textures              = {};
        Shaders shaders                = {};
        ShaderPrograms shader_programs = {};
        DrawQueue draw_queue           = {};
        Animations animations          = {};
      };
    }
  }
}
