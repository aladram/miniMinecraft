#pragma once

#include <array>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>

namespace opengl_demo {
    struct chunk
    {
        block get_block(const vector3i& loc) const;
        void set_block(const vector3i& loc, const block& block);

        static constexpr size_t N = 16;
        std::array<block, N * N * N> blocks;
    };
    using chunk_t = chunk;
}
