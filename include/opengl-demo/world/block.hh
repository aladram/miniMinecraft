#pragma once

#include <array>
#include <cstdint>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>

namespace opengl_demo {
    typedef struct texture_ids
    {
        texture_ids(float id)
            : side_id{id}
            , top_id{id}
            , bottom_id{id}
        {}

        texture_ids(float _side_id, float _top_id, float _bottom_id)
            : side_id{_side_id}
            , top_id{_top_id}
            , bottom_id{_bottom_id}
        {}

        float side_id;
        float top_id;
        float bottom_id;
    } texture_ids_t;

    enum class block_type
    {
		AIR,
		STONE,
		GRASS,
		DIRT,
		COBBLESTONE,
		WOOD,
		SAPLING,
		BEDROCK,
		WATER,
		STATIONARY_WATER,
		LAVA,
		STATIONARY_LAVA,
		SAND,
		GRAVEL,
		GOLD_ORE,
		IRON_ORE,
		COAL_ORE,
		LOG,
		LEAVES,
		SPONGE,
		GLASS,
		LAPIS_ORE,
		LAPIS_BLOCK,
		DISPENSER,
		SANDSTONE,
		NOTE_BLOCK,
		WOOL,
		YELLOW_FLOWER,
		RED_ROSE,
		BROWN_MUSHROOM,
		RED_MUSHROOM,
		GOLD_BLOCK,
		IRON_BLOCK,
		DOUBLE_STEP,
		STEP,
		BRICK,
		TNT,
		BOOKSHELF,
		MOSSY_COBBLESTONE,
		OBSIDIAN
    };
    using block_type_t = block_type;

    struct gl_block
    {
        vector3 position;
        // 0: side, 1: top, 2: bottom
        texture_ids_t texture_ids;
    };
    using gl_block_t = gl_block;

    struct block
    {
        aabb hitbox() const;
        gl_block to_opengl() const;
        bool opaque() const;

        vector3 position;
        block_type type;
        bool visible = false;
    };
    using block_t = block;
}
