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
