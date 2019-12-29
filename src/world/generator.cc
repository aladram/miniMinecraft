#include <opengl-demo/world/generator.hh>

#include <cassert>
#include <climits>
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

    double operator()(double x, double y) const
    {
        double result = 0.0;
        double amp = 1.0;

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

    double operator()(double x, double y) const
    {
        return noise1(x + noise2(x, y), y);
    }

    const octave_noise noise1;
    const octave_noise noise2;
};

static std::vector<unsigned> generate_height_map(unsigned size)
{
    assert(size <= (unsigned)INT_MAX);

    static constexpr unsigned sea_level = 62;

    std::random_device r;
    const combined_noise noise1{r(), r(), 8};
    const combined_noise noise2{r(), r(), 8};
    const octave_noise noise3{r(), 6};

    std::vector<unsigned> height_map(size * size);

#pragma omp parallel for
    for (unsigned i = 0; i < size * size; ++i)
    {
        double x = (double) ((int) (i / size) - (int)size / 2);
        double z = (double) ((int) (i % size) - (int)size / 2);

        double low = noise1(x * 1.3, z * 1.3) / 6. - 4.;
        double high = noise2(x * 1.3, z * 1.3) / 5. + 6.;

        double height = ((noise3(x,z) > 0.) ? low : std::max(low, high)) / 2.;
        if (height < 0)
            height *= 0.8;

        height_map[i] = (unsigned)height + sea_level;
    }

    return height_map;
}

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

    world_t world{player, {}};

    constexpr unsigned size = 256;
    const auto height_map = generate_height_map(size);
    for (unsigned i = 0; i < size; ++i)
        for (unsigned j = 0; j < size; ++j)
        {
            unsigned height = height_map[i * size + j];
            int x = (int)i - (int)size / 2;
            int z = (int)j - (int)size / 2;

            for (unsigned y = 0; y < height - 1; ++y)
                world.set_block_unsafe({ x, y, z }, block{ { x, y, z }, block_type::DIRT });
            world.set_block_unsafe({ x, height - 1, z }, block{ { x, height - 1, z }, block_type::GRASS });
        }

    // Compute visible blocks
    for (auto& chunk: world.chunks)
        for (auto& block: chunk.second.blocks)
            // Check if block is hidden
            block.visible = block.opaque() && (world.immediate_neighbors(block.position).size() != 6);

    world.player.position.y = height_map[size * (size / 2 + (int)world.player.position.x) + (size / 2 + (int)world.player.position.z)] + 3;

    return world;
}
