#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace opengl_demo
{
    struct camera
    {
        glm::vec3 position;
        glm::vec3 forward_;
        glm::vec3 up_;

        glm::vec3 forward() const
        {
            return glm::normalize(forward_);
        }

        glm::vec3 up() const
        {
            return glm::normalize(up_);
        }

        glm::vec3 right() const
        {
            return glm::normalize(glm::cross(forward_, up_));
        }

        auto look_at() const
        {
            return glm::lookAt(
                    position,
                    position + forward_,
                    up_
                );
        }
    };
    using camera_t = camera;
}
