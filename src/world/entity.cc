#include <opengl-demo/world/entity.hh>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

void entity::update_side(world& world, unsigned i)
{
    for (const auto& block: world.neighbors(position, 7))
    {
        if (hitbox().intersects(block.hitbox()))
        {
            if ((int) block.position[i] == (int) std::floor(position[i] - 0.5f))
            {
                velocity[i] = 0;
                position[i] = block.position[i] + 1;
            }

            if ((int) block.position[i] == (int) std::floor(position[i] + measurements[i] + 0.5f))
            {
                velocity[i] = 0;
                position[i] = block.position[i] - measurements[i];
            }
        }
    }
}

void entity::update(world& world, float dt)
{
    constexpr vector3 net_force = { 0.f, 2 * -9.81f, 0.f };
    constexpr float attenuation = 0.2f;

    velocity = std::max(1.0f - dt * attenuation, 0.f) * velocity + dt * net_force;
    position += dt * velocity;

    // For now, only updating Y axis
    update_side(world, 1);
}
