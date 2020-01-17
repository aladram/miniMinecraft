#include <opengl-demo/world/world.hh>

#include <mutex>
#include <shared_mutex>
#include <vector>

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
        chunk_it = chunks.insert({loc, chunk_t(loc)}).first;

    return chunk_it->second;
}

block world::get_block(const vector3i& loc) const
{
    std::shared_lock<std::shared_mutex> lock(*mutex);

    auto chunk_it = chunks.find(loc_of_chunk(loc));
    // Return air if chunk does not exist
    if (chunk_it == chunks.end())
        return block{loc, block_type::AIR};

    const auto& chunk = chunk_it->second;
    return chunk.get_block(loc_in_chunk(loc));
}

void world::set_block_unsafe(const vector3i& loc, const block& block)
{
    std::unique_lock<std::shared_mutex> lock(*mutex);

    auto& chunk = get_chunk(loc_of_chunk(loc));
    chunk.set_block(loc_in_chunk(loc), block);

    // TODO: remove chunk if block == air and chunk now empty
}

void world::set_block_unsafe(const vector3i& loc, block_type type)
{
    set_block_unsafe(loc, block{loc, type});
}

void world::update_visibility(const vector3i& loc)
{
    auto block = get_block(loc);
    auto neighbors = immediate_neighbors(loc);

    for (auto b: neighbors)
    {
        b.visible = b.opaque() && (immediate_neighbors(b.position).size() != 6);
        set_block_unsafe(b.position, b);
    }
    block.visible = block.opaque() && (neighbors.size() != 6);
    set_block_unsafe(block.position, block);
}

void world::set_block(const vector3i& loc, const block& block)
{
    set_block_unsafe(loc, block);

    update_visibility(loc);
}

void world::set_block(const vector3i& loc, block_type type)
{
    set_block(loc, block{loc, type});
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
                if (block.type != block_type::AIR)
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
        if (block.opaque())
            neighborhood.push_back(block);
    }

    return neighborhood;
}

std::vector<block> world::get_blocks(const vector3i& from, const vector3i& to) const
{
    assert(from.x <= to.x);
    assert(from.y <= to.y);
    assert(from.z <= to.z);

    std::vector<block> blocks;
    blocks.reserve((to.x - from.x) * (to.y - from.y) * (to.z - from.z));

    for (int x = from.x; x < to.x; ++x)
        for (int y = from.y; y < to.y; ++y)
            for (int z = from.z; z < to.z; ++z)
            {
                const auto& block = get_block({x, y, z});
                if (block.type != block_type::AIR)
                    blocks.push_back(block);
            }

    return blocks;
}
