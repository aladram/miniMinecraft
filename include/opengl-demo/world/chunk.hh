#pragma once

#include <array>
#include <shared_mutex>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>

namespace opengl_demo {
    struct chunk
    {
        chunk(const vector3i& loc);
        block get_block(const vector3i& loc) const;
        void set_block(const vector3i& loc, const block& block);
        bool is_cached() const;

        static constexpr size_t N = 16;
        std::array<block, N * N * N> blocks;
        std::array<std::vector<gl_block>, 3> gl_cache;
        std::unique_ptr<std::shared_mutex> mutex = std::make_unique<std::shared_mutex>();
    };
    using chunk_t = chunk;
}
