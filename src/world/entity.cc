#include <opengl-demo/world/entity.hh>

#include <cstdio>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/aabb.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

void entity::update(world& world, float dt)
{
    constexpr vector3 net_force = { 0.f, 2 * -9.81f, 0.f };
    constexpr float attenuation = 0;//.2f;

    velocity = std::max(1.0f - dt * attenuation, 0.f) * velocity + dt * net_force;
    position += dt * velocity;

    for (auto& block: world.blocks)
    {
        for (unsigned i: { 1, 0, 2 })
        {
            if (hitbox().intersects(block.hitbox()))
            {
                if (velocity[i] <= 0 && (int) block.position[i] == (int) std::floor(position[i] - 0.5f))
                {
                    if (i != 1)
                    {
                        printf("%f %f %f\n", position.x, position.y, position.z);
                        block.texture_ids = { 129 + i / 2, 129 + i / 2, 129 + i / 2 };
                    }
                    velocity[i] = 0;
                    position[i] = block.position[i] + 1;
                }

                if (velocity[i] >= 0 && (int) block.position[i] == (int) std::floor(position[i] + measurements[i] + 0.5f))
                {
                    if (i != 1)
                        block.texture_ids = { 129 + 48 + i / 2, 129 + 48 + i / 2, 129 + 48 + i / 2 };
                    velocity[i] = 0;
                    position[i] = block.position[i] - measurements[i];
                }
            }
        }
    }
}
