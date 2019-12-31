#pragma once

#include <memory>
#include <shared_mutex>
#include <vector>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/chunk.hh>
#include <opengl-demo/world/entity.hh>

namespace opengl_demo {
    struct world
    {
        chunk& get_chunk(const vector3i& loc);
        block get_block(const vector3i& loc) const;
        void set_block_unsafe(const vector3i& loc, const block& block);
        void set_block_unsafe(const vector3i& loc, block_type type);
        void update_visibility(const vector3i& loc);
        void set_block(const vector3i& loc, const block& block);
        void set_block(const vector3i& loc, block_type type);
        std::vector<block> neighbors(const vector3i& loc, unsigned diameter) const;
        std::vector<block> immediate_neighbors(const vector3i& loc) const;
        std::vector<block> get_blocks(const vector3i& from, const vector3i& to) const;

        entity player;
        std::unordered_map<vector3i, chunk, vector3i_hash> chunks;
        std::unique_ptr<std::shared_mutex> mutex = std::make_unique<std::shared_mutex>();
    };
    using world_t = world;
}
