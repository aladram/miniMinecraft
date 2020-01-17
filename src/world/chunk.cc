#include <opengl-demo/world/chunk.hh>

#include <cassert>
#include <cstddef>
#include <shared_mutex>
#include <mutex>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>

using namespace opengl_demo;

static size_t index(const vector3i& loc)
{
    static constexpr size_t N = chunk_t::N;

    assert(loc.x >= 0 && loc.x < N);
    assert(loc.y >= 0 && loc.y < N);
    assert(loc.z >= 0 && loc.z < N);

    return (size_t) (loc.x * (N * N) + loc.y * N + loc.z);
}

chunk::chunk(const vector3i& loc)
{
    vector3i base_loc = loc * (int)N;

    for (size_t x = 0; x < N; ++x)
        for (size_t y = 0; y < N; ++y)
            for (size_t z = 0; z < N; ++z)
            {
                vector3i off = vector3i(x, y, z);
                blocks[index(off)] = block_t{base_loc + off, block_type::AIR};
            }
}

block chunk::get_block(const vector3i& loc) const
{
    std::unique_lock<std::shared_mutex> lock(*mutex);

    return blocks[index(loc)];
}

void chunk::set_block(const vector3i& loc, const block& block)
{
    std::shared_lock<std::shared_mutex> lock(*mutex);

    blocks[index(loc)] = block;
}
