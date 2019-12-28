#pragma once

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>

namespace opengl_demo {
    struct world;

    struct entity
    {
        aabb hitbox() const
        {
            return aabb{position, position + measurements};
        }

        vector3 eyes_position() const
        {
            return position + eyes_offset;
        }

        void update(world& world, float dt);

        vector3 measurements;
        vector3 position;
        vector3 eyes_offset;
        vector3 velocity = { 0.f, 0.f, 0.f };
    };
    using entity_t = entity;
}
