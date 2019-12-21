#pragma once

#include <vector>

#include <opengl-demo/world/block.hh>

namespace opengl_demo {
    struct world
    {
        std::vector<block> blocks;
    };

    world generate_world();
}
