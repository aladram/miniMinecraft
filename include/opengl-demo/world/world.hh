#pragma once

#include <vector>

#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/entity.hh>

namespace opengl_demo {
    struct world
    {
        void init();
        void render() const;

        std::vector<block> blocks;
        entity player;
    };

    world generate_world();
}
