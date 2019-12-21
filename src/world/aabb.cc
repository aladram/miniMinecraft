#include <opengl-demo/world/aabb.hh>

using namespace opengl_demo;

bool aabb::intersects(const aabb& other) const
{
    return (p_min.x <= other.p_max.x && p_max.x >= other.p_min.x)
        && (p_min.y <= other.p_max.y && p_max.y >= other.p_min.y)
        && (p_min.z <= other.p_max.z && p_max.z >= other.p_min.z);
}
