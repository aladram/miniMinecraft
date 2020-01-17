#pragma once

#include <optional>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/world.hh>

namespace opengl_demo
{
    struct camera
    {
        vector3 position;
        vector3 forward_;
        vector3 up_;
        std::optional<vector3> target_loc = std::nullopt;
        std::optional<vector3> target_normal = std::nullopt;

        vector3 forward() const;
        vector3 up() const;
        vector3 right() const;
        glm::mat4x4 look_at() const;
        std::optional<block> target_block(const world& world) const;
        std::optional<vector3> target_free_loc(const world& world) const;
    };
    using camera_t = camera;
}
