#include <opengl-demo/world/aabb.hh>

#include <climits>

using namespace opengl_demo;

bool aabb::intersects(const aabb& other) const
{
    return (p_min.x + FLT_EPSILON < other.p_max.x && p_max.x - FLT_EPSILON > other.p_min.x)
        && (p_min.y + FLT_EPSILON < other.p_max.y && p_max.y - FLT_EPSILON > other.p_min.y)
        && (p_min.z + FLT_EPSILON < other.p_max.z && p_max.z - FLT_EPSILON > other.p_min.z);
}
