#include <opengl-demo/world/entity.hh>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

void entity::update(const world& world, float dt)
{
    constexpr vector3 net_force = { 0.f, -9.81f, 0.f };
    constexpr float attenuation = 0.2f;

    velocity = (1.0f - dt * attenuation) * velocity + dt * net_force;
    vector3 new_pos = position + dt * velocity;

    for (const auto& block: world.blocks)
    {
        if (hitbox().intersects(block.hitbox()))
        {
            velocity = { 0.f, 0.f, 0.f };
            return;
        }
    }

    position = new_pos;
}
