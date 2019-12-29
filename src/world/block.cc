#include <opengl-demo/world/block.hh>

#include <cassert>
#include <unordered_map>

#include <opengl-demo/math.hh>

using namespace opengl_demo;

aabb block::hitbox() const
{
    return aabb{position, position + vector3{1.f, 1.f, 1.f}};
}

bool block::opaque() const
{
    return type != block_type::AIR;
}

gl_block block::to_opengl() const
{
    static const std::unordered_map<block_type, texture_ids_t> textures_ids{
		{ block_type::STONE, { 1 } },
		{ block_type::GRASS, { 3, 40, 2 } },
		{ block_type::DIRT, { 2 } },
		{ block_type::COBBLESTONE, { 16 } },
		{ block_type::WOOD, { 4 } },
		{ block_type::BEDROCK, { 17 } },
		{ block_type::WATER, { 205 } },
		{ block_type::LAVA, { 237 } },
		{ block_type::SAND, { 18 } },
		{ block_type::GRAVEL, { 0 } },
		{ block_type::GOLD_ORE, { 32 } },
		{ block_type::IRON_ORE, { 33 } },
		{ block_type::COAL_ORE, { 34 } },
		{ block_type::LOG, { 20, 21, 21 } },
		{ block_type::LEAVES, { 212 } },
		{ block_type::SPONGE, { 48 } },
		{ block_type::GLASS, { 49 } },
		{ block_type::LAPIS_ORE, { 160 } },
		{ block_type::LAPIS_BLOCK, { 144 } },
		{ block_type::SANDSTONE, { 192, 176, 208 } },
		{ block_type::NOTE_BLOCK, { 74, 75, 74 } },
		{ block_type::WOOL, { 64 } },
		{ block_type::GOLD_BLOCK, { 23 } },
		{ block_type::IRON_BLOCK, { 22 } },
		{ block_type::BRICK, { 7 } },
		{ block_type::TNT, { 8, 9, 10 } },
		{ block_type::BOOKSHELF, { 35, 4, 4 } },
		{ block_type::MOSSY_COBBLESTONE, { 36 } },
		{ block_type::OBSIDIAN, { 37 } }
    };

    assert(type != block_type::AIR);

    return gl_block{
        position,
#ifdef NDEBUG
        textures_ids[type]
#else
        textures_ids.at(type)
#endif
    };
}
