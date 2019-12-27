#pragma once

#include <array>
#include <cstdint>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>

namespace opengl_demo {
    using texture_id_t = float;
    using texture_ids_t = std::array<texture_id_t, 3>;

    struct block
    {
        static const block air;

        aabb hitbox() const
        {
            return aabb{position, position + vector3{1.f, 1.f, 1.f}};
        }

        vector3 position;
        // 0: side, 1: top, 2: bottom
        texture_ids_t texture_ids;
    };
    using block_t = block;
}
