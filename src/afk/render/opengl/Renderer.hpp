#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
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
#include "afk/render/GlfwContext.hpp"
#include "afk/render/Model.hpp"
#include "afk/render/Shader.hpp"
#include "afk/render/opengl/MeshHandle.hpp"
#include "afk/render/opengl/ModelHandle.hpp"
#include "afk/render/opengl/ShaderHandle.hpp"
#include "afk/render/opengl/ShaderProgramHandle.hpp"
#include "afk/render/opengl/TextureHandle.hpp"

namespace afk {
  namespace render {
    struct Model;
    struct Mesh;
    struct WireframeMesh;
    struct Model;
    struct Texture;
    struct ShaderProgram;

    namespace opengl {
      /**
       * An OpenGL 4.1 renderer implementation.
       */
      class Renderer {
      public:
        /** The mesh handle type. */
        using MeshHandle = opengl::MeshHandle;
        /** The model handle type. */
        using ModelHandle = opengl::ModelHandle;
        /** The shader handle type. */
        using ShaderHandle = opengl::ShaderHandle;
        /** The shader program handle type. */
        using ShaderProgramHandle = opengl::ShaderProgramHandle;
        /** The texture handle type. */
        using TextureHandle = opengl::TextureHandle;

        /**
         * Struct responsible for hashing paths.
         */
        struct PathHash {
          /**
           * Hashes a std::filesystem::path.
           * @param p The path to hash.
           * @return The resulting hash.
           */
          auto operator()(const std::filesystem::path &p) const -> usize {
            return std::filesystem::hash_value(p);
          }
        };

        /**
         * Struct responsible for checking path equality.
         */
        struct PathEquals {
          /**
           * Compares the specified lhs and rhs paths in order to check for
           * lexically normal equality.
           *
           * @param lhs The left hand side path.
           * @param rhs The right hand side path.
           */
          auto operator()(const std::filesystem::path &lhs,
                          const std::filesystem::path &rhs) const -> bool {
            return lhs.lexically_normal() == rhs.lexically_normal();
          }
        };

        /**
         * A struct encapsulating a request to draw a model.
         */
        struct DrawCommand {
          /** The path of the model to draw. */
          const std::filesystem::path model_path = {};
          /** The shader program to use while drawing. */
          const std::filesystem::path shader_program_path = {};
          /** The transformation to apply. */
          const physics::Transform transform = {};
        };

        /** A map of model paths to loaded model handles. */
        using Models =
            std::unordered_map<std::filesystem::path, ModelHandle, PathHash, PathEquals>;
        /** A map of texture paths to loaded texture handles. */
        using Textures =
            std::unordered_map<std::filesystem::path, TextureHandle, PathHash, PathEquals>;
        /** A map of shader paths to loaded shader handles. */
        using Shaders =
            std::unordered_map<std::filesystem::path, ShaderHandle, PathHash, PathEquals>;
        /** A map of shader program paths to loaded shader program handles. */
        using ShaderPrograms =
            std::unordered_map<std::filesystem::path, ShaderProgramHandle, PathHash, PathEquals>;
        /** A map of animation paths to loaded animation handles. */
        using Animations =
            std::unordered_map<std::filesystem::path, Model::Animations, PathHash, PathEquals>;

        /** The underlying GLFW window type. */
        using Window       = std::shared_ptr<GLFWwindow>;
        using WindowHandle = Window::weak_type;

      private:
        /** The GLFW context. */
        GlfwContext glfw_context = {};

      public:
        /** The window being drawn to. */
        Window window = nullptr;

        Renderer();
        Renderer(Renderer &&)      = delete;
        Renderer(const Renderer &) = delete;
        auto operator=(const Renderer &) -> Renderer & = delete;
        auto operator=(Renderer &&) -> Renderer & = delete;

        /**
         * Initializes this renderer.
         */
        auto initialize() -> void;

        /**
         * Sets a specified OpenGL option to the specified state.
         *
         * @param option The OpenGL option to set.
         * @param state The state; true or false.
         */
        auto set_option(GLenum option, bool state) const -> void;

        /**
         * Returns the current window size.
         *
         * @return The current window size.
         */
        auto get_window_size() const -> glm::ivec2;

        /**
         * Clears the screen with the specified color.
         *
         * @param clear_color The color to clear the screen with.
         */
        auto clear_screen(glm::vec4 clear_color = {255.0f, 255.0f, 255.0f, 1.0f}) const
            -> void;

        /**
         * Swaps the front and back framebuffers.
         */
        auto swap_buffers() -> void;

        /**
         * Sets the rendering viewport to the specified x, y, width, and height.
         *
         * @param x The viewport x.
         * @param y The viewport y.
         * @param width The viewport width.
         * @param height The viewport height.
         */
        auto set_viewport(i32 x, i32 y, i32 width, i32 height) const -> void;

        /**
         * Draws the specified model with the specified shader
         * program and transformation.
         *
         * @param model The model to draw.
         * @param shader_program The shader program to use.
         * @param transform The model transformation.
         */
        auto draw_model(const ModelHandle &model, const ShaderProgramHandle &shader_program,
                        physics::Transform transform) const -> void;

        auto draw_wireframe_mesh(const WireframeMesh &mesh,
                                 const ShaderProgramHandle &shader_program) const -> void;

        /**
         * Sets up the projection and view matrices.
         *
         * @param shader_program The shader program to use.
         */
        auto setup_view(const ShaderProgramHandle &shader_program) const -> void;

        /**
         * Enables the specified shader.
         *
         * @param shader The shader to enable.
         */
        auto use_shader(const ShaderProgramHandle &shader) const -> void;

        /**
         * Sets the current texture unit.
         *
         * @param unit The texture unit.
         */
        auto set_texture_unit(usize unit) const -> void;

        /**
         * Binds the specified texture.
         *
         * @param texture The texture to bind.
         */
        auto bind_texture(const TextureHandle &texture) const -> void;

        /**
         * Returns a model handle corresponding to the specified model path,
         * if the model is not loaded it is loaded then returned.
         *
         * @param file_path The model path.
         * @return The loaded model handle.
         */
        auto get_model(const std::filesystem::path &file_path) -> const ModelHandle &;

        /**
         * Returns a texture handle corresponding to the specified texture path,
         * if the model is not loaded it is loaded then returned.
         *
         * @param file_path The texture path.
         * @return The loaded texture handle.
         */
        auto get_texture(const std::filesystem::path &file_path) -> const TextureHandle &;

        /**
         * Returns a shader handle corresponding to the specified shader path,
         * if the model is not loaded it is loaded then returned.
         *
         * @param file_path The shader path.
         * @return The loaded shader handle.
         */
        auto get_shader(const std::filesystem::path &file_path) -> const ShaderHandle &;

        /**
         * Returns a shader program handle corresponding to the specified shader
         * program path, if the model is not loaded it is loaded then returned.
         *
         * @param file_path The shader program path.
         * @return The loaded shader program handle.
         */
        auto get_shader_program(const std::filesystem::path &file_path)
            -> const ShaderProgramHandle &;

        /**
         * Loads the specified model and returns a model handle for use.
         *
         * @param model The model to load.
         * @return The resulting model handle.
         */
        auto load_model(const Model &model) -> ModelHandle;

        /**
         * Loads the specified texture and returns a texture handle for use.
         *
         * @param texture The texture to load.
         * @return The resulting texture handle.
         */
        auto load_texture(const Texture &texture) -> TextureHandle;

        /**
         * Loads the specified mesh and returns a mesh handle for use.
         *
         * @param mesh The mesh to load.
         * @return The resulting mesh handle.
         */
        auto load_mesh(const Mesh &mesh) -> MeshHandle;

        /**
         * Compiles a shader and returns a shader handle for use.
         *
         * @param shader The shader to load.
         * @return The resulting shader handle.
         */
        auto compile_shader(const Shader &shader) -> ShaderHandle;

        /**
         * Links a shader program and returns a shader program handle for use.
         *
         * @param shader_program The shader program to link.
         */
        auto link_shaders(const ShaderProgram &shader_program) -> ShaderProgramHandle;

        /**
         * @name shader_uniforms
         */
        //@{

        /**
         * Sets a uniform shader value.
         *
         * @param program The shader program handle to use.
         * @param name The uniform name.
         * @param value The uniform value.
         */
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
        //@}

        /**
         * Sets if the wireframe is enabled or not.
         *
         * @param status The status to use.
         */
        auto set_wireframe(bool status) -> void;

        /**
         * Returns the current wireframe status.
         */
        auto get_wireframe() const -> bool;

        /**
         * Returns the model handle container.
         */
        auto get_models() const -> const Models &;

        /**
         * Returns the texture handle container.
         */
        auto get_textures() const -> const Textures &;

        /**
         * Returns the shader handle container.
         */
        auto get_shaders() const -> const Shaders &;
        /**
         * Returns the shader program handle container.
         */
        auto get_shader_programs() const -> const ShaderPrograms &;

      private:
        /** The OpenGL major version being used. */
        static constexpr i32 opengl_major_version = 4;
        /** The OpenGL minor version being used. */
        static constexpr i32 opengl_minor_version = 1;

        /** Is the renderer initialized? */
        bool is_initialized = false;
        /** Is the wireframe enabled? */
        bool wireframe_enabled = false;

        /** The model cache. */
        Models models = {};
        /** The texture cache. */
        Textures textures = {};
        /** The shader cache. */
        Shaders shaders = {};
        /** The shader program cache. */
        ShaderPrograms shader_programs = {};
        /** The animation cache. */
        Animations animations = {};
      };
    }
  }
}
