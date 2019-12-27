#pragma once

#include <cstddef>
#include <functional>

#include <glm/glm.hpp>

namespace opengl_demo {
    using vector3 = glm::vec3;
    using vector3i = glm::ivec3;

    struct vector3i_hash
    {
        std::size_t operator()(const vector3i& v) const noexcept
        {
            return hash_int(v.x) ^ (hash_int(v.y) << 1) ^ (hash_int(v.z) << 2);
        }

        const std::hash<vector3i::value_type> hash_int;
    };
}
