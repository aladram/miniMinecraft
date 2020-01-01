#include <opengl-demo/world/generator.hh>

#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include <PerlinNoise.hpp>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>

using namespace opengl_demo;

struct octave_noise
{
    octave_noise(std::uint32_t seed, std::int32_t _octaves)
        : noise(seed)
        , octaves(_octaves)
    {}

    float operator()(float x, float y) const
    {
        float result = 0.0;
        float amp = 1.0;

        for (std::int32_t i = 0; i < octaves; ++i)
        {
            result += noise.noise(x, y) * amp;
            x *= 0.5;
            y *= 0.5;
            amp *= 2.;
        }

        return result;
    }

    const siv::PerlinNoise noise;
    const std::int32_t octaves;
};

struct combined_noise
{
    combined_noise(std::uint32_t seed1, std::uint32_t seed2, std::int32_t octaves)
        : noise1(seed1, octaves)
        , noise2(seed2, octaves)
    {}

    float operator()(float x, float y) const
    {
        return noise1(x + noise2(x, y), y);
    }

    const octave_noise noise1;
    const octave_noise noise2;
};

static std::vector<int> generate_height_map(std::random_device& r, unsigned size, int sea_level)
{
    assert(size <= (unsigned)std::numeric_limits<int>::max());

    const combined_noise noise1{r(), r(), 8};
    const combined_noise noise2{r(), r(), 8};
    const octave_noise noise3{r(), 6};

    std::vector<int> height_map(size * size);

#pragma omp parallel for
    for (unsigned i = 0; i < size * size; ++i)
    {
        float x = (float) ((int) (i / size) - (int)size / 2);
        float z = (float) ((int) (i % size) - (int)size / 2);

        float low = noise1(x * 1.3, z * 1.3) / 6. - 4.;
        float high = noise2(x * 1.3, z * 1.3) / 5. + 6.;

        float height = ((noise3(x,z) > 0.) ? low : std::max(low, high)) / 2.;
        if (height < 0)
            height *= 0.8;

        height_map[i] = (int)height + sea_level;
    }

    return height_map;
}

static unsigned height_map_idx(unsigned size, int x, int z)
{
    return size * (size / 2 + x) + (size / 2 + z);
}

static void fill_sphere_from_stone(world& world, const vector3i& center, int r, block_type type)
{
    for (int dx = -r; dx < r; ++dx)
        for (int dy = -r; dy < r; ++dy)
            for (int dz = -r; dz < r; ++dz)
            {
                vector3i off(dx, dy, dz);
                if (glm::length(vector3(off)) > r)
                    continue;
                vector3i loc = center + off;
                if (world.get_block(loc).type == block_type::STONE)
                    world.set_block_unsafe(loc, type);
            }
}

/*
 * Generation algorithm described on
 * https://github.com/UnknownShadow200/ClassiCube/wiki/Minecraft-Classic-map-generation-algorithm
 */
world opengl_demo::generate_world()
{
    entity player{
        // Measurements
        { 0.6, 1.8, 0.6 },
        // Position
        { -3.f, 82.f, -3.f },
        // Eyes offset
        { 0.3, 1.7, 0.3 }
    };

    static constexpr unsigned size = 256;
    static constexpr int sea_level = 62;
    static constexpr unsigned world_height = 128;
    static constexpr unsigned world_width = size;
    static constexpr unsigned world_depth = size;

    std::random_device r;
    std::mt19937 gen{r()};
    std::uniform_real_distribution<> unit_distrib;
    const auto random = [&gen, &unit_distrib] () { return unit_distrib(gen); };
    std::uniform_int_distribution<int> x_distrib{- (int)world_width / 2, (int)world_width / 2 - 1};
    std::uniform_int_distribution<int> y_distrib{0, (int)world_height - 1};
    std::uniform_int_distribution<int> z_distrib{- (int)world_depth / 2, (int)world_depth / 2 - 1};
    const auto random_pos = [&gen, &x_distrib, &y_distrib, &z_distrib] {
        return vector3i{
            x_distrib(gen),
            y_distrib(gen),
            z_distrib(gen)
        };
    };

    // Height map creation
    const auto height_map = generate_height_map(r, size, sea_level);

    world_t world{player, {}};

    // Terrain creation
    {
        const octave_noise noise{r(), 8};

#pragma omp parallel for
        for (unsigned i = 0; i < size; ++i)
            for (unsigned j = 0; j < size; ++j)
            {
                int height = height_map[i * size + j];
                int x = (int)i - (int)size / 2;
                int z = (int)j - (int)size / 2;

                int stone_height = height + (int) (noise(x, z) / 24. - 3.);

                world.set_block_unsafe({ x, 0, z }, block_type::BEDROCK);
                for (int y = 1; y < stone_height; ++y)
                    world.set_block_unsafe({ x, y, z }, block_type::STONE);
                for (int y = stone_height; y < height - 1; ++y)
                    world.set_block_unsafe({ x, y, z }, block_type::DIRT);
            }
    }

    // Caving
    {
        unsigned caves = (world_height * world_width * world_depth) / 8192;

#pragma omp parallel for schedule(dynamic)
        for (unsigned i = 0; i < caves; ++i)
        {
            vector3i cave = random_pos();
            unsigned length = (unsigned) (random() * random() * 200.f);

            float theta = random() * 2 * M_PI;
            float phi = random() * 2 * M_PI;
            float dtheta = 0, dphi = 0;
            float radius = random() * random();

            for (unsigned l = 0; l < length; ++l)
            {
                cave += vector3(
                        std::sin(theta) * std::cos(phi),
                        std::cos(theta) * std::cos(phi),
                        std::sin(phi)
                    );

                theta += dtheta * 0.2f;
                phi = phi / 2.f + dphi / 4.f;
                dtheta = dtheta * 0.9f + random() - random();
                dphi = dphi * 0.75f + random() - random();

                if (random() > 0.75f)
                    continue;

                vector3i center = cave + vector3i(
                        (random() * 4.f - 2.f) * 0.2f,
                        (random() * 4.f - 2.f) * 0.2f,
                        (random() * 4.f - 2.f) * 0.2f
                    );

                int r = (int) (1.2f + ((float) ((int)world_height - cave.y) / (float)world_height * 3.5f + 1.f) * radius * std::sin((float)l * M_PI / (float)length));
                // Replace stone by air in sphere of radius r
                fill_sphere_from_stone(world, center, r, block_type::AIR);
            }
        }
    }

    // Ore veins generation
    using ores_list = std::initializer_list<std::pair<block_type, float>>;
    for (auto [ore_type, abundance] : ores_list{{block_type::COAL_ORE, 0.9f}, {block_type::IRON_ORE, 0.7f}, {block_type::GOLD_ORE, 0.5f}})
    {
        unsigned veins = (world_height * world_width * world_depth) / 16384;

#pragma omp parallel for schedule(dynamic)
        for (unsigned i = 0; i < veins; ++i)
        {
            vector3i vein = random_pos();
            unsigned length = (unsigned) (random() * random() * 75.f * abundance);

            float theta = random() * 2 * M_PI;
            float phi = random() * 2 * M_PI;
            float dtheta = 0, dphi = 0;

            for (unsigned l = 0; l < length; ++l)
            {
                vein += vector3(
                        std::sin(theta) * std::cos(phi),
                        std::cos(theta) * std::cos(phi),
                        std::sin(phi)
                    );

                theta += dtheta * 0.2f;
                phi = phi / 2.f + dphi / 4.f;
                dtheta = dtheta * 0.9f + random() - random();
                dphi = dphi * 0.9f + random() - random();

                int r = (int) (abundance * std::sin((float)l * M_PI / (float)length) + 1.);
                // Replace stone by air in sphere of radius r
                fill_sphere_from_stone(world, vein, r, ore_type);
            }
        }
    }

    // Filling water
    {
#pragma omp parallel for
        for (unsigned i = 0; i < size; ++i)
            for (unsigned j = 0; j < size; ++j)
            {
                int height = height_map[i * size + j];

                int x = (int)i - (int)size / 2;
                int z = (int)j - (int)size / 2;

                for (int y = height; y < sea_level; ++y)
                    world.set_block_unsafe({ x, y, z }, block_type::WATER);
            }
    }

    // TODO: flood caves with water randomly
    // TODO: flood caves with lava randomly

    // Surface generation
    {
        const octave_noise noise1{r(), 8};
        const octave_noise noise2{r(), 8};

#pragma omp parallel for
        for (unsigned i = 0; i < size; ++i)
            for (unsigned j = 0; j < size; ++j)
            {
                int height = height_map[i * size + j];

                int x = (int)i - (int)size / 2;
                int z = (int)j - (int)size / 2;

                bool sand = (noise1(x,z) > 8.);
                bool gravel = (noise2(x,z) > 12.);

                auto type_above = world.get_block({x, height, z }).type;

                if (type_above == block_type::WATER && gravel)
                    world.set_block_unsafe({ x, height - 1, z }, block_type::GRAVEL);
                else if (type_above == block_type::AIR)
                {
                    if (height - 1 <= sea_level && sand)
                        world.set_block_unsafe({ x, height - 1, z }, block_type::SAND);
                    else
                        world.set_block_unsafe({ x, height - 1, z }, block_type::GRASS);
                }

            }
    }

    // Trees generation
    {
        unsigned forests = (world_width * world_depth) / 4000;

#pragma omp parallel for schedule(dynamic)
        for (unsigned i = 0; i < forests; ++i)
        {
            vector3i forest = random_pos();

            for (unsigned j = 0; j < 20; ++j)
            {
                vector3i tree = forest;

                for (unsigned k = 0; k < 20; ++k)
                {
                    tree += vector3i(
                            (int) (6.f * random()) - (int) (6.f * random()),
                            0,
                            (int) (6.f * random()) - (int) (6.f * random())
                        );

                    if (tree.x < - (int)size / 2 || tree.x >= (int)size / 2)
                        continue;
                    if (tree.z < - (int)size / 2 || tree.z >= (int)size / 2)
                        continue;

                    if (random() > 0.25f)
                        continue;

                    tree.y = height_map[height_map_idx(size, tree.x, tree.z)];
                    int height = (int) (random() * 3.f + 4.f);

                    if (!world.get_blocks(
                                tree + vector3i(-1, 0, -1),
                                tree + vector3i(2, height - 4, 2)
                            ).empty())
                        continue;
                    if (!world.get_blocks(
                                tree + vector3i(-2, height - 4, -2),
                                tree + vector3i(3, height, 3)
                            ).empty())
                        continue;

                    // [0, height - 4[
                    for (int y_off = 0; y_off < height - 4; ++y_off)
                        world.set_block_unsafe(tree + vector3i(0, y_off, 0), block_type::LOG);
                    // height - 4 and height - 3
                    for (auto y_off: { height - 4, height - 3 })
                    {
                        for (int x_off = -2; x_off < 3; ++x_off)
                            for (int z_off = -2; z_off < 3; ++z_off)
                                world.set_block_unsafe(tree + vector3i(x_off, y_off, z_off), block_type::LEAVES);
                        world.set_block_unsafe(tree + vector3i(0, y_off, 0), block_type::LOG);
                        world.set_block_unsafe(
                                tree + vector3i(-2, y_off, -2),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                        world.set_block_unsafe(
                                tree + vector3i(-2, y_off, 2),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                        world.set_block_unsafe(
                                tree + vector3i(2, y_off, -2),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                        world.set_block_unsafe(
                                tree + vector3i(2, y_off, 2),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                    }
                    // height - 2
                    {
                        int y_off = height - 2;
                        for (int x_off = -1; x_off < 2; ++x_off)
                            for (int z_off = -1; z_off < 2; ++z_off)
                                world.set_block_unsafe(tree + vector3i(x_off, y_off, z_off), block_type::LEAVES);
                        world.set_block_unsafe(tree + vector3i(0, y_off, 0), block_type::LOG);
                        world.set_block_unsafe(
                                tree + vector3i(-1, y_off, -1),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                        world.set_block_unsafe(
                                tree + vector3i(-1, y_off, 1),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                        world.set_block_unsafe(
                                tree + vector3i(1, y_off, -1),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                        world.set_block_unsafe(
                                tree + vector3i(1, y_off, 1),
                                (random() < 0.5) ? block_type::LEAVES : block_type::AIR
                            );
                    }
                    // height - 1
                    {
                        int y_off = height - 1;
                        world.set_block_unsafe(tree + vector3i(0, y_off, 0), block_type::LEAVES);
                        world.set_block_unsafe(tree + vector3i(-1, y_off, 0), block_type::LEAVES);
                        world.set_block_unsafe(tree + vector3i(1, y_off, 0), block_type::LEAVES);
                        world.set_block_unsafe(tree + vector3i(0, y_off, -1), block_type::LEAVES);
                        world.set_block_unsafe(tree + vector3i(0, y_off, 1), block_type::LEAVES);
                    }
                }
            }
        }
    }

    // Compute visible blocks
    for (auto& chunk: world.chunks)
#pragma omp parallel for schedule(dynamic)
        for (auto& block: chunk.second.blocks)
            // Check if block is hidden
            block.visible = block.opaque() && (world.immediate_neighbors(block.position).size() != 6);

    const auto highest_point_idx = std::max_element(height_map.begin(), height_map.end()) - height_map.begin();
    world.player.position.x = (float) (highest_point_idx / size - size / 2);
    world.player.position.z = (float) (highest_point_idx % size - size / 2);
    world.player.position.y = height_map[height_map_idx(size, (int) world.player.position.x, (int) world.player.position.z)] + 3;

    return world;
}
