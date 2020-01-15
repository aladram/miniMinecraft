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

// Using Multiple Render Target to fetch target_loc from renderer
std::optional<block> camera::target_block(const world& world) const
{
    static constexpr float max_dist = 6.f;

    if (distance(world.player.position, target_loc) > max_dist)
        return std::nullopt;

    auto block = world.get_block(target_loc);
    if (block.type == block_type::AIR)
        return std::nullopt;

    return block;
}
