#include "Bindings.hpp"
extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}
#include <LuaBridge/LuaBridge.h>

#include "afk/Afk.hpp"
#include "afk/asset/Asset.hpp"
#include "afk/asset/AssetFactory.hpp"
#include "afk/component/BaseComponent.hpp"
#include "afk/component/GameObject.hpp"
#include "afk/component/ScriptsComponent.hpp"
#include "afk/io/ModelSource.hpp"
#include "afk/physics/PhysicsBody.hpp"
#include "afk/physics/Transform.hpp"
#include "afk/renderer/Camera.hpp"
#include "afk/script/Script.hpp"
#include "afk/ui/Ui.hpp"

// todo move to keyboard mgmt
#include <GLFW/glfw3.h>

static auto get_camera() -> Afk::Camera & {
  return Afk::Engine::get().camera;
}

static auto get_delta_time() -> float {
  return Afk::Engine::get().get_delta_time();
}

struct GameObjectWrapped {
  Afk::GameObject e;
  template<typename T>
  auto get_component() -> T * {
    return Afk::Engine::get().registry.try_get<T>(this->e);
  }
};

static auto get_owning_entity(Afk::LuaScript *self) -> GameObjectWrapped {
  return GameObjectWrapped{self->my_owner->owning_entity};
}
static auto script_data(GameObjectWrapped *entity, const std::string &path) -> LuaRef {
  return entity->get_component<Afk::ScriptsComponent>()->get_script_table(path);
}

template<typename T>
static auto get_parent(T *bc) -> ENTT_ID_TYPE {
  return static_cast<ENTT_ID_TYPE>(bc->owning_entity);
}

template<typename VecType>
static auto vec_add(VecType *a, VecType b) -> VecType {
  return *a + b;
}
template<typename VecType>
static auto vec_sub(VecType *a, VecType b) -> VecType {
  return *a - b;
}
template<typename VecType>
static auto vec_smul(VecType *a, float f) -> VecType {
  return *a * f;
}
template<typename VecType>
static auto vec_dot(VecType *a, VecType rhs) -> float {
  return glm::dot(*a, rhs);
}
template<typename VecType>
static auto vec_cross(VecType *a, VecType rhs) -> VecType {
  return glm::cross(*a, rhs);
}
template<typename VecType>
static auto vec_normal(VecType *a) -> VecType {
  return glm::normalize(*a);
}

// todo move to keyboard mgmt
static auto key_pressed(int key_code) -> bool {
  return glfwGetKey(Afk::Engine::get().renderer.window, key_code) == GLFW_PRESS;
}

static auto gameobject_get_entity(Afk::Asset::Asset *e) -> GameObjectWrapped {
  return GameObjectWrapped{std::get<Afk::Asset::Asset::Object>(e->data).ent};
}

static auto toggle_wireframe() -> void {
  auto &renderer = Afk::Engine::get().renderer;
  renderer.set_wireframe(!renderer.get_wireframe());
}
static auto toggle_menu() -> void {
  auto &ui     = Afk::Engine::get().ui;
  ui.show_menu = !ui.show_menu;
}

using namespace luabridge;
auto Afk::add_engine_bindings(lua_State *lua) -> void {

  getGlobalNamespace(lua)
      .beginClass<glm::vec3>("vector3")
      .addConstructor<void (*)(float, float, float)>()
      .addData("x", &glm::vec3::x)
      .addData("y", &glm::vec3::y)
      .addData("z", &glm::vec3::z)
      .addFunction("normalized", &vec_normal<glm::vec3>)
      .addFunction("add", &vec_add<glm::vec3>)
      .addFunction("sub", &vec_sub<glm::vec3>)
      .addFunction("smul", &vec_smul<glm::vec3>)
      .addFunction("dot", &vec_dot<glm::vec3>)
      .addFunction("cross", &vec_cross<glm::vec3>)
      .endClass()

      .beginNamespace("math")
      .addFunction("clamp", &std::clamp<float>)
      .endNamespace()

      .beginClass<glm::vec2>("vector2")
      .addConstructor<void (*)(float, float)>()
      .addData("x", &glm::vec2::x)
      .addData("y", &glm::vec2::y)
      .addFunction("normalized", &vec_normal<glm::vec2>)
      .addFunction("add", &vec_add<glm::vec2>)
      .addFunction("sub", &vec_sub<glm::vec2>)
      .addFunction("smul", &vec_smul<glm::vec2>)
      .addFunction("dot", &vec_dot<glm::vec2>)
      .endClass()

      .beginClass<glm::quat>("quaternion")
      .addConstructor<void (*)(float, float, float, float)>()
      .addProperty("x", &glm::quat::x)
      .addProperty("y", &glm::quat::y)
      .addProperty("z", &glm::quat::y)
      .addProperty("w", &glm::quat::w)
      .endClass()

      .beginNamespace("ui")
      .addFunction("toggle_menu", &toggle_menu)
      .endNamespace()

      .beginClass<Afk::Camera>("camera")
      .addStaticFunction("current", &get_camera)
      .addProperty("pos", &Afk::Camera::get_position, &Afk::Camera::set_position)
      .addProperty("angle", &Afk::Camera::get_angles, &Afk::Camera::set_angles)
      .addFunction("front", &Afk::Camera::get_front)
      .addFunction("right", &Afk::Camera::get_right)
      .addFunction("up", &Afk::Camera::get_up)
      .endClass()

      .beginClass<GameObjectWrapped>("entity")
      .addFunction("get_transform", &GameObjectWrapped::get_component<Transform>)
      .addFunction("get_physics", &GameObjectWrapped::get_component<PhysicsBody>)
      .addFunction("get_model", &GameObjectWrapped::get_component<ModelSource>)
      .addFunction("get_script", &GameObjectWrapped::get_component<ScriptsComponent>)
      .addFunction("script_data", &script_data)
      .endClass()

      .beginClass<Afk::PhysicsBody>("physics_component")
      .addFunction("parent", &get_parent<Afk::PhysicsBody>)
      .addFunction("apply_force", &Afk::PhysicsBody::apply_force)
      .addFunction("apply_torque", &Afk::PhysicsBody::apply_torque)
      .addFunction("translate", &Afk::PhysicsBody::translate)
      .endClass()

      .beginClass<Afk::Transform>("transform_component")
      .addFunction("parent", &get_parent<Afk::Transform>)
      .addData("translation", &Afk::Transform::translation)
      .addData("rotation", &Afk::Transform::rotation)
      .addData("scale", &Afk::Transform::scale)
      .endClass()

      .beginClass<Afk::ScriptsComponent>("script_component")
      .addFunction("parent", &get_parent<Afk::ScriptsComponent>)
      .addFunction("add", &Afk::ScriptsComponent::add_script)
      .addFunction("remove", &Afk::ScriptsComponent::remove_script)
      .addFunction("get_data", &Afk::ScriptsComponent::get_script_table)
      .endClass()

      .beginClass<Afk::ModelSource>("model_component")
      .addFunction("parent", &get_parent<Afk::ModelSource>)
      .addData("name", &Afk::ModelSource::name)
      .endClass()

      .beginClass<Afk::BaseComponent>("component")
      .addFunction("parent", &get_parent<Afk::BaseComponent>)
      .endClass()

      .beginNamespace("keyboard")
      .addFunction("is_pressed", &key_pressed)
      .endNamespace()

      .beginClass<Afk::Asset::Asset>("asset")
      .addFunction("unwrap_entity", &gameobject_get_entity)
      .endClass()

      .beginNamespace("engine")
      .addFunction("delta_time", &get_delta_time)
      .addFunction("load_asset", &Afk::Asset::game_asset_factory)
      .addFunction("toggle_wireframe", &toggle_wireframe)
      .endNamespace();

  auto key_ns = luabridge::getGlobalNamespace(lua).beginNamespace("key");
  for (const auto &key : Afk::Script::keys) {
    // key.code can't be changed from lua's side
    key_ns.addVariable<int>(key.name.c_str(), const_cast<int *>(&key.code), false);
  }
  key_ns.endNamespace();

  auto mouse_ns = luabridge::getGlobalNamespace(lua).beginNamespace("mouse");
  for (const auto &mouse_button : Afk::Script::mouse_buttons) {
    mouse_ns.addVariable<int>(mouse_button.name.c_str(),
                              const_cast<int *>(&mouse_button.button), false);
  }
  mouse_ns.endNamespace();

  luabridge::getGlobalNamespace(lua)
      .beginNamespace("events")
      .beginClass<Afk::Event::Key>("key")
      .addData("alt", &Afk::Event::Key::alt, false)
      .addData("control", &Afk::Event::Key::control, false)
      .addData("shift", &Afk::Event::Key::shift, false)
      .addData("key", &Afk::Event::Key::key, false)
      .addData("key_code", &Afk::Event::Key::scancode, false)
      .endClass()
      .beginClass<Afk::Event::MouseMove>("mouse_move")
      .addData("x", &Afk::Event::MouseMove::x, false)
      .addData("y", &Afk::Event::MouseMove::y, false)
      .endClass()
      .beginClass<Afk::Event::MouseButton>("mouse_button")
      .addData("alt", &Afk::Event::MouseButton::alt, false)
      .addData("control", &Afk::Event::MouseButton::control, false)
      .addData("shift", &Afk::Event::MouseButton::shift, false)
      .addData("button", &Afk::Event::MouseButton::button, false)
      .endClass()
      .beginClass<Afk::Event::MouseScroll>("mouse_scroll")
      .addData("x", &Afk::Event::MouseScroll::x, false)
      .addData("y", &Afk::Event::MouseScroll::y, false)
      .endClass()
      .beginClass<Afk::Event::Text>("text")
      .addData("codepoint", &Afk::Event::Text::codepoint, false)
      .endClass()
      .beginClass<Afk::Event::Update>("update")
      .addData("delta", &Afk::Event::Update::dt, false)
      .endClass()
      .endNamespace();

  auto afk_event_class =
      luabridge::getGlobalNamespace(lua).beginClass<Afk::Event>("event");
  for (const auto &event : Afk::Script::events) {
    afk_event_class.addStaticData<int>(event.name.c_str(),
                                       const_cast<int *>(&event.type), false);
  }
  afk_event_class.addFunction("to_key", &Afk::Event::get<Afk::Event::Key>);
  afk_event_class.addFunction("to_mouse_move", &Afk::Event::get<Afk::Event::MouseMove>);
  afk_event_class.addFunction("to_mouse_button", &Afk::Event::get<Afk::Event::MouseButton>);
  afk_event_class.addFunction("to_mouse_scroll", &Afk::Event::get<Afk::Event::MouseScroll>);
  afk_event_class.addFunction("to_text", &Afk::Event::get<Afk::Event::Text>);
  afk_event_class.addFunction("to_update", &Afk::Event::get<Afk::Event::Update>);
  afk_event_class.endClass();

  auto script_class = luabridge::getGlobalNamespace(lua)
                          .beginClass<LuaScript>("__AFK__SCRIPT")
                          .addFunction("register_event", &Afk::LuaScript::register_fn)
                          .addFunction("entity", &get_owning_entity)
                          .endClass();
}
