#pragma once

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>
#include <opengl-demo/world/world.hh>

namespace opengl_demo {
    struct entity
    {
        entity(const vector3& _measurements, const vector3& _position)
            : measurements{_measurements}
            , position{_position}
        {}

        aabb hitbox() const
        {
            return aabb{position, position + measurements};
        }

        void update(const world& world, float dt);

        vector3 measurements;
        vector3 position;
        vector3 velocity = { 0.f, 0.f, 0.f };
    };
}
