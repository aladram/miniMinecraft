#include <opengl-demo/world/entity.hh>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

void entity::update(world& world, float dt)
{
    constexpr vector3 net_force = { 0.f, 2 * -9.81f, 0.f };
    constexpr float attenuation = 0.2f;

    vector3 old_pos = position;

    velocity = std::max(1.0f - dt * attenuation, 0.f) * velocity + dt * net_force;
    position += dt * velocity;

    for (const auto& block: world.neighbors(position, 7))
    {
        std::array<unsigned, 3> axes = { 1, 0, 2 };
        unsigned height = std::floor(old_pos.y);

        float dx = old_pos.x - (block.position.x + 0.5);
        float dz = old_pos.z - (block.position.z + 0.5);
        if (std::abs(old_pos.y - (float)height) > 0.01)
        {
            if (std::abs(dz) > std::abs(dx))
                axes = { 2, 0, 1 };
            else
                axes = { 0, 2, 1 };
        }
        else if (std::abs(dz) > std::abs(dx))
            axes = { 1, 2, 0 };

        for (unsigned i: axes)
        {
            if (hitbox().intersects(block.hitbox()))
            {
                if (velocity[i] <= 0 && (int) block.position[i] == (int) std::floor(position[i] - 0.5f))
                {
                    velocity[i] = 0;
                    position[i] = block.position[i] + 1;
                }

                if (velocity[i] >= 0 && (int) block.position[i] == (int) std::floor(position[i] + measurements[i] + 0.5f))
                {
                    velocity[i] = 0;
                    position[i] = block.position[i] - measurements[i];
                }
            }
        }
    }
}
