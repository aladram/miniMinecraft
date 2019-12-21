#pragma once

#include <array>
#include <cstdint>

#include <opengl-demo/math.hh>

namespace opengl_demo {
    using texture_id_t = float;
    using texture_ids_t = std::array<texture_id_t, 3>;

    struct block
    {
        vector3 position;
        // 0: side, 1: top, 2: bottom
        texture_ids_t texture_ids;
    };
}
