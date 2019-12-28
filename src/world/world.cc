#include <opengl-demo/world/world.hh>

#include <cassert>
#include <climits>
#include <random>
#include <utility>
#include <vector>

#include <PerlinNoise.hpp>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/chunk.hh>

using namespace opengl_demo;

static vector3i loc_in_chunk(const vector3i& loc)
{
    return (loc % (int)chunk_t::N + vector3i((int)chunk_t::N)) % (int)chunk_t::N;
}

static vector3i loc_of_chunk(const vector3i& loc)
{
    return (loc - loc_in_chunk(loc)) / (int)chunk_t::N;
}

chunk& world::get_chunk(const vector3i& loc)
{
    auto chunk_it = chunks.find(loc);
    // Create new chunk if it does not exist
    if (chunk_it == chunks.end())
        chunks[loc] = chunk_t{};

    return chunks[loc];
}

block world::get_block(const vector3i& loc) const
{
    auto chunk_it = chunks.find(loc_of_chunk(loc));
    // Return air if chunk does not exist
    if (chunk_it == chunks.end())
        return block::air;

    const auto& chunk = chunk_it->second;
    return chunk.get_block(loc_in_chunk(loc));
}

void world::set_block_unsafe(const vector3i& loc, const block& block)
{
    auto& chunk = get_chunk(loc_of_chunk(loc));
    chunk.set_block(loc_in_chunk(loc), block);

    // TODO: remove chunk if block == air and chunk now empty
}

void world::update_visibility(const vector3i& loc)
{
    auto block = get_block(loc);
    auto neighbors = immediate_neighbors(loc);

    for (auto b: neighbors)
    {
        b.visible = b.texture_ids[0] && (immediate_neighbors(b.position).size() != 6);
        set_block_unsafe(b.position, b);
    }
    block.visible = block.texture_ids[0] && (neighbors.size() != 6);
    set_block_unsafe(block.position, block);
}

void world::set_block(const vector3i& loc, const block& block)
{
    set_block_unsafe(loc, block);

    update_visibility(loc);
}

std::vector<block> world::neighbors(const vector3i& loc, unsigned diameter) const
{
    std::vector<block> neighborhood;
    neighborhood.reserve(diameter * diameter * diameter);

    for (unsigned i = 0; i < diameter; ++i)
        for (unsigned j = 0; j < diameter; ++j)
            for (unsigned k = 0; k < diameter; ++k)
            {
                vector3i off = vector3i(i, j, k) - vector3i(diameter / 2);
                const auto& block = get_block(loc + off);
                // Check if not air block
                if (block.texture_ids[0])
                    neighborhood.push_back(block);
            }

    return neighborhood;
}

std::vector<block> world::immediate_neighbors(const vector3i& loc) const
{
    static constexpr std::array<vector3i, 6> offsets{{
        { -1, 0, 0 },
        { 1, 0, 0 },
        { 0, 0, -1 },
        { 0, 0, 1 },
        { 0, -1, 0 },
        { 0, 1, 0 }
    }};

    std::vector<block> neighborhood;
    neighborhood.reserve(6);

    for (const auto& off: offsets)
    {
        const auto& block = get_block(loc + off);
        // Check if not air block
        if (block.texture_ids[0])
            neighborhood.push_back(block);
    }

    return neighborhood;
}

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
    constexpr texture_ids_t dirt_texture = { 2, 2, 2 };
    constexpr texture_ids_t grass_texture = { 3, 40, 2 };
    for (unsigned i = 0; i < size; ++i)
        for (unsigned j = 0; j < size; ++j)
        {
            unsigned height = height_map[i * size + j];
            int x = (int)i - (int)size / 2;
            int z = (int)j - (int)size / 2;

            for (unsigned y = 0; y < height - 1; ++y)
                world.set_block_unsafe({ x, y, z }, block{ { x, y, z }, dirt_texture });
            world.set_block_unsafe({ x, height - 1, z }, block{ { x, height - 1, z }, grass_texture });
        }

    // Compute visible blocks
    for (auto& chunk: world.chunks)
        for (auto& block: chunk.second.blocks)
            // Check if block is hidden
            block.visible = (block.texture_ids[0]) && (world.immediate_neighbors(block.position).size() != 6);

    world.player.position.y = height_map[size * (size / 2 + (int)world.player.position.x) + (size / 2 + (int)world.player.position.z)] + 3;

    return world;
}
