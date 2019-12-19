#pragma once

#include <cstdint>

#include <opengl-demo/math.hh>

namespace opengl_demo {
    using texture_id_t = float;

    struct block
    {
        vector3 position;
        // 0: side, 1: top, 2: bottom
        texture_id_t texture_ids[3];
    };
}
