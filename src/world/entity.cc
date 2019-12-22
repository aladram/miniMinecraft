#include <opengl-demo/world/entity.hh>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

void entity::update(const world& world, float dt)
{
    constexpr vector3 net_force = { 0.f, 2 * -9.81f, 0.f };
    constexpr float attenuation = 0;//.2f;

    velocity = std::max(1.0f - dt * attenuation, 0.f) * velocity + dt * net_force;
    position += dt * velocity;

    for (const auto& block: world.blocks)
    {
        if (hitbox().intersects(block.hitbox()))
        {
            // Floor collision
            if (velocity.y < 0
                    && (int) block.position.y == (int) std::floor(position.y - 0.5f))
            {
                velocity.y = 0;
                position.y = block.position.y + 1;
            }

            // Ceil collision
            if (velocity.y > 0
                    && (int) block.position.y == (int) std::floor(position.y + measurements.y + 0.5f))
            {
                velocity.y = 0;
                position.y = block.position.y - measurements.y;
            }
        }
    }
}
