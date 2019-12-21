#pragma once

#include <opengl-demo/math.hh>

namespace opengl_demo {
    struct aabb
    {
        aabb(const vector3& a, const vector3& b)
            : p_min{std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)}
            , p_max{std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)}
        {}

        bool intersects(const aabb& other) const;

        vector3 p_min;
        vector3 p_max;
    };
}
