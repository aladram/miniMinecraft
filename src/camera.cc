#include <opengl-demo/camera.hh>

#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opengl-demo/math.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

vector3 camera::forward() const
{
    return glm::normalize(forward_);
}

vector3 camera::up() const
{
    return glm::normalize(up_);
}

vector3 camera::right() const
{
    return glm::normalize(glm::cross(forward_, up_));
}

glm::mat4x4 camera::look_at() const
{
    return glm::lookAt(
            position,
            position + forward_,
            up_
        );
}

// Raymarching
std::optional<block> camera::target_block(const world& world) const
{
    static constexpr float min_dist = 0.1f;
    static constexpr float max_dist = 6.f;
    static constexpr float marching_step = 0.01f;

    for (float dist = min_dist; dist < max_dist; dist += marching_step)
    {
        vector3 loc = position + forward_ * dist;
        // Handle cast of negative float to int
        if (loc.x < 0)
            loc.x -= 1;
        if (loc.z < 0)
            loc.z -= 1;
        auto block = world.get_block(loc);
        // Check if block not air
        if (block.texture_ids[0])
            return block;
    }

    return std::nullopt;
}
