#include <opengl-demo/world/world.hh>

#include <utility>
#include <vector>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>

using namespace opengl_demo;

world opengl_demo::generate_world()
{
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
    constexpr int map_radius = 100;
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

    return world{std::move(blocks)};
}
