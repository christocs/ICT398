#pragma once

#include <variant>

#include <entt/entt.hpp>

namespace Afk::Asset {
  /**
   * Asset type
   */
  enum class AssetType { Terrain, Object };
  /**
   * Asset
   */
  struct Asset {
    /**
     * Asset data
     */
    struct Object {
      entt::entity ent;
    };
    struct Terrain {};
    typedef std::variant<std::monostate, Asset::Object, Asset::Terrain> AssetData;
    AssetData data;
    AssetType asset_type;
  };
}
