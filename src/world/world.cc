#include <opengl-demo/world/world.hh>

#include <utility>
#include <vector>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/chunk.hh>

using namespace opengl_demo;

static vector3i loc_chunk(const vector3i& loc)
{
    return (loc % (int)chunk_t::N + vector3i((int)chunk_t::N)) % (int)chunk_t::N;
}

static vector3i loc_world(const vector3i& loc)
{
    return (loc - loc_chunk(loc)) / (int)chunk_t::N;
}

block world::get_block(const vector3i& loc) const
{
    auto chunk_it = chunks.find(loc / (int)chunk_t::N);
    // Return air if chunk does not exist
    if (chunk_it == chunks.end())
        return block::air;

    const auto& chunk = chunk_it->second;
    return chunk.get_block(loc_chunk(loc));
}

void world::set_block(const vector3i& loc, const block& block)
{
    auto chunk_it = chunks.find(loc_world(loc));
    // Create new chunk if it does not exist
    if (chunk_it == chunks.end())
        chunks[loc_world(loc)] = chunk_t{};

    auto& chunk = chunks[loc_world(loc)];
    chunk.set_block(loc_chunk(loc), block);
}

world opengl_demo::generate_world()
{
    entity player{
        // Measurements
        { 0.6, 1.8, 0.6 },

        // Position
        { -3.f, 82.f, -3.f }
    };

    /*
     * Bookshelf structure generation
     */
    std::vector<block> blocks = {
        { { 0, 0, 0 }, { 35, 4, 4 } },
        { { 0, 0, 1 }, { 35, 4, 4 } },
        { { 0, 0, 2 }, { 35, 4, 4 } },
        { { 0, 0, 3 }, { 35, 4, 4 } },
        { { 0, 1, 0 }, { 35, 4, 4 } },
        { { 0, 1, 1 }, { 35, 4, 4 } },
        { { 0, 1, 2 }, { 35, 4, 4 } },
        { { 0, 2, 0 }, { 35, 4, 4 } },
        { { 0, 2, 1 }, { 35, 4, 4 } },
        { { 0, 3, 0 }, { 35, 4, 4 } },
        { { 1, 0, 0 }, { 35, 4, 4 } },
        { { 1, 0, 1 }, { 35, 4, 4 } },
        { { 1, 0, 2 }, { 35, 4, 4 } },
        { { 1, 1, 0 }, { 35, 4, 4 } },
        { { 1, 1, 1 }, { 35, 4, 4 } },
        { { 1, 2, 0 }, { 35, 4, 4 } },
        { { 2, 0, 0 }, { 35, 4, 4 } },
        { { 2, 0, 1 }, { 35, 4, 4 } },
        { { 2, 1, 0 }, { 35, 4, 4 } },
        { { 3, 0, 0 }, { 35, 4, 4 } }
    };
    for (auto& block: blocks)
        block.position += vector3{ 0, 70, 0 };

    /*
     * Flat terrain generation
     */
    constexpr int map_radius = 50;
    constexpr int terrain_height = 70;
    constexpr int terrain_height_start = 67;
    constexpr texture_ids_t dirt_texture = { 2, 2, 2 };
    constexpr texture_ids_t grass_texture = { 3, 40, 2 };
    for (int i = -map_radius; i < map_radius; ++i)
    {
        for (int j = -map_radius; j < map_radius; ++j)
        {
            for (int k = terrain_height_start; k < terrain_height - 1; ++k)
                blocks.push_back(block{{i, k, j}, dirt_texture});

            blocks.push_back(block{{i, terrain_height - 1, j}, grass_texture});
        }
    }

    world_t world{player, {}, {}};
    for (const auto& block: blocks)
        world.set_block(vector3i(block.position), block);

    // test
    for (const auto& chunk: world.chunks)
        for (const auto& block: chunk.second.blocks)
            // Check if not air
            if (block.texture_ids[0])
                world.blocks.push_back(block);

    return world;
}
