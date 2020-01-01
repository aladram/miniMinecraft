#include <opengl-demo/primitives/cube.hh>

#include <cstddef>

#include <glad/glad.h>

#include <opengl-demo/vertex_attributes.hh>

using namespace opengl_demo;

static GLuint generate_vao(float* data, size_t size)
{
    GLuint my_vao, my_vbo;
    glGenVertexArrays(1, &my_vao);
    glGenBuffers(1, &my_vbo);

    glBindVertexArray(my_vao);
      glBindBuffer(GL_ARRAY_BUFFER, my_vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glVertexAttribPointer(ATTRIB_CUBE_VERTICES, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
        glVertexAttribPointer(ATTRIB_CUBE_UVS, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
        glVertexAttribPointer(ATTRIB_CUBE_FACES_TYPES, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (5 * sizeof(float)));
        glEnableVertexAttribArray(ATTRIB_CUBE_VERTICES);
        glEnableVertexAttribArray(ATTRIB_CUBE_UVS);
        glEnableVertexAttribArray(ATTRIB_CUBE_FACES_TYPES);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return my_vao;
}

GLuint opengl_demo::generate_cube_vao()
{
    float data[] = {
        // Vertices        // UVs     // Face type (0: side, 1: top, 2: bottom)

        // Face 1 (front)
        // Triangle 1
        0.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        0.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        // Triangle 2
        0.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        0.f, 1.f, 0.f,     0.f, 1.f,  0.f,

        // Face 2 (right)
        // Triangle 1
        0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        // Triangle 2
        0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        0.f, 1.f, 1.f,     0.f, 1.f,  0.f,

        // Face 3 (back)
        // Triangle 1
        1.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 0.f, 0.f,     1.f, 0.f,  0.f,
        1.f, 1.f, 0.f,     1.f, 1.f,  0.f,
        // Triangle 2
        1.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 1.f, 0.f,     1.f, 1.f,  0.f,
        1.f, 1.f, 1.f,     0.f, 1.f,  0.f,

        // Face 4 (left)
        // Triangle 1
        1.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.f, 0.f, 0.f,     1.f, 0.f,  0.f,
        0.f, 1.f, 0.f,     1.f, 1.f,  0.f,
        // Triangle 2
        1.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.f, 1.f, 0.f,     1.f, 1.f,  0.f,
        1.f, 1.f, 0.f,     0.f, 1.f,  0.f,

        // Face 5 (top)
        // Triangle 1
        0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        0.f, 1.f, 1.f,     1.f, 0.f,  1.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  1.f,
        // Triangle 2
        0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  1.f,
        1.f, 1.f, 0.f,     0.f, 1.f,  1.f,

        // Face 6 (bottom)
        // Triangle 1
        1.f, 0.f, 0.f,     0.f, 0.f,  2.f,
        1.f, 0.f, 1.f,     1.f, 0.f,  2.f,
        0.f, 0.f, 1.f,     1.f, 1.f,  2.f,
        // Triangle 2
        1.f, 0.f, 0.f,     0.f, 0.f,  2.f,
        0.f, 0.f, 1.f,     1.f, 1.f,  2.f,
        0.f, 0.f, 0.f,     0.f, 1.f,  2.f
    };

    return generate_vao(data, sizeof(data));
}

GLuint opengl_demo::generate_cube_mirror_vao()
{
    float data[] = {
        // Vertices        // UVs     // Face type (0: side, 1: top, 2: bottom)

        // Face 1 (front)
        // Triangle 1
        0.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        0.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        // Triangle 2
        0.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        0.f, 1.f, 0.f,     0.f, 1.f,  0.f,

        // Face 2 (right)
        // Triangle 1
        0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        // Triangle 2
        0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  0.f,
        0.f, 1.f, 1.f,     0.f, 1.f,  0.f,

        // Face 3 (back)
        // Triangle 1
        1.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        1.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        1.f, 1.f, 0.f,     0.f, 1.f,  0.f,
        // Triangle 2
        1.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        1.f, 1.f, 0.f,     0.f, 1.f,  0.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  0.f,

        // Face 4 (left)
        // Triangle 1
        1.f, 0.f, 0.f,     1.f, 0.f,  0.f,
        0.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.f, 1.f, 0.f,     0.f, 1.f,  0.f,
        // Triangle 2
        1.f, 0.f, 0.f,     1.f, 0.f,  0.f,
        0.f, 1.f, 0.f,     0.f, 1.f,  0.f,
        1.f, 1.f, 0.f,     1.f, 1.f,  0.f,

        // Face 5 (top)
        // Triangle 1
        0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        0.f, 1.f, 1.f,     1.f, 0.f,  1.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  1.f,
        // Triangle 2
        0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        1.f, 1.f, 1.f,     1.f, 1.f,  1.f,
        1.f, 1.f, 0.f,     0.f, 1.f,  1.f,

        // Face 6 (bottom)
        // Triangle 1
        1.f, 0.f, 0.f,     0.f, 1.f,  2.f,
        1.f, 0.f, 1.f,     1.f, 1.f,  2.f,
        0.f, 0.f, 1.f,     1.f, 0.f,  2.f,
        // Triangle 2
        1.f, 0.f, 0.f,     0.f, 1.f,  2.f,
        0.f, 0.f, 1.f,     1.f, 0.f,  2.f,
        0.f, 0.f, 0.f,     0.f, 0.f,  2.f
    };

    return generate_vao(data, sizeof(data));
}
