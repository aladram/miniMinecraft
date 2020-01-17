#include <opengl-demo/primitives/cube.hh>

#include <cstddef>

#include <glad/glad.h>

#include <opengl-demo/vertex_attributes.hh>

using namespace opengl_demo;

struct vertex
{
    float position[3];
    float normal[3];
    float uv[2];
    float face_type;
};
using vertex_t = vertex;

struct triangle
{
    vertex_t vertices[3];
};
using triangle_t = triangle;

static GLuint generate_vao(float* data, size_t size)
{
    GLuint my_vao, my_vbo;
    glGenVertexArrays(1, &my_vao);
    glGenBuffers(1, &my_vbo);

    glBindVertexArray(my_vao);
      glBindBuffer(GL_ARRAY_BUFFER, my_vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glVertexAttribPointer(ATTRIB_CUBE_VERTICES, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, position));
        glVertexAttribPointer(ATTRIB_CUBE_NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, normal));
        glVertexAttribPointer(ATTRIB_CUBE_UVS, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, uv));
        glVertexAttribPointer(ATTRIB_CUBE_FACES_TYPES, 1, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, face_type));
        glEnableVertexAttribArray(ATTRIB_CUBE_VERTICES);
        glEnableVertexAttribArray(ATTRIB_CUBE_NORMALS);
        glEnableVertexAttribArray(ATTRIB_CUBE_UVS);
        glEnableVertexAttribArray(ATTRIB_CUBE_FACES_TYPES);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return my_vao;
}

GLuint opengl_demo::generate_cube_vao()
{
    float data[] = {
        // Vertices        // Normals         // UVs     // Face type (0: side, 1: top, 2: bottom)

        // Face 1 (front)
        // Triangle 1
        0.f, 0.f, 0.f,     -1.f, 0.f, 0.f,    0.f, 0.f,  0.f,
        0.f, 0.f, 1.f,     -1.f, 0.f, 0.f,    1.f, 0.f,  0.f,
        0.f, 1.f, 1.f,     -1.f, 0.f, 0.f,    1.f, 1.f,  0.f,
        // Triangle 2
        0.f, 0.f, 0.f,     -1.f, 0.f, 0.f,    0.f, 0.f,  0.f,
        0.f, 1.f, 1.f,     -1.f, 0.f, 0.f,    1.f, 1.f,  0.f,
        0.f, 1.f, 0.f,     -1.f, 0.f, 0.f,    0.f, 1.f,  0.f,

        // Face 2 (right)
        // Triangle 1
        0.f, 0.f, 1.f,     0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 0.f, 1.f,     0.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        1.f, 1.f, 1.f,     0.f, 0.f, 1.f,     1.f, 1.f,  0.f,
        // Triangle 2
        0.f, 0.f, 1.f,     0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        1.f, 1.f, 1.f,     0.f, 0.f, 1.f,     1.f, 1.f,  0.f,
        0.f, 1.f, 1.f,     0.f, 0.f, 1.f,     0.f, 1.f,  0.f,

        // Face 3 (back)
        // Triangle 1
        1.f, 0.f, 1.f,     1.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        1.f, 0.f, 0.f,     1.f, 0.f, 0.f,     1.f, 0.f,  0.f,
        1.f, 1.f, 0.f,     1.f, 0.f, 0.f,     1.f, 1.f,  0.f,
        // Triangle 2
        1.f, 0.f, 1.f,     1.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        1.f, 1.f, 0.f,     1.f, 0.f, 0.f,     1.f, 1.f,  0.f,
        1.f, 1.f, 1.f,     1.f, 0.f, 0.f,     0.f, 1.f,  0.f,

        // Face 4 (left)
        // Triangle 1
        1.f, 0.f, 0.f,     0.f, 0.f, -1.f,    0.f, 0.f,  0.f,
        0.f, 0.f, 0.f,     0.f, 0.f, -1.f,    1.f, 0.f,  0.f,
        0.f, 1.f, 0.f,     0.f, 0.f, -1.f,    1.f, 1.f,  0.f,
        // Triangle 2
        1.f, 0.f, 0.f,     0.f, 0.f, -1.f,    0.f, 0.f,  0.f,
        0.f, 1.f, 0.f,     0.f, 0.f, -1.f,    1.f, 1.f,  0.f,
        1.f, 1.f, 0.f,     0.f, 0.f, -1.f,    0.f, 1.f,  0.f,

        // Face 5 (top)
        // Triangle 1
        0.f, 1.f, 0.f,     0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        0.f, 1.f, 1.f,     0.f, 1.f, 0.f,     1.f, 0.f,  1.f,
        1.f, 1.f, 1.f,     0.f, 1.f, 0.f,     1.f, 1.f,  1.f,
        // Triangle 2
        0.f, 1.f, 0.f,     0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        1.f, 1.f, 1.f,     0.f, 1.f, 0.f,     1.f, 1.f,  1.f,
        1.f, 1.f, 0.f,     0.f, 1.f, 0.f,     0.f, 1.f,  1.f,

        // Face 6 (bottom)
        // Triangle 1
        1.f, 0.f, 0.f,     0.f, -1.f, 0.f,    0.f, 0.f,  2.f,
        1.f, 0.f, 1.f,     0.f, -1.f, 0.f,    1.f, 0.f,  2.f,
        0.f, 0.f, 1.f,     0.f, -1.f, 0.f,    1.f, 1.f,  2.f,
        // Triangle 2
        1.f, 0.f, 0.f,     0.f, -1.f, 0.f,    0.f, 0.f,  2.f,
        0.f, 0.f, 1.f,     0.f, -1.f, 0.f,    1.f, 1.f,  2.f,
        0.f, 0.f, 0.f,     0.f, -1.f, 0.f,    0.f, 1.f,  2.f
    };

    return generate_vao(data, sizeof(data));
}

GLuint opengl_demo::generate_cube_mirror_vao()
{
    float data[] = {
        // Vertices        // Normals         // UVs     // Face type (0: side, 1: top, 2: bottom)

        // Face 1 (front)
        // Triangle 1
        0.001f, 0.001f, 0.001f,     -1.f, 0.f, 0.f,    0.f, 0.f,  0.f,
        0.001f, 0.001f, 0.999f,     -1.f, 0.f, 0.f,    1.f, 0.f,  0.f,
        0.001f, 0.999f, 0.999f,     -1.f, 0.f, 0.f,    1.f, 1.f,  0.f,
        // Triangle 2
        0.001f, 0.001f, 0.001f,     -1.f, 0.f, 0.f,    0.f, 0.f,  0.f,
        0.001f, 0.999f, 0.999f,     -1.f, 0.f, 0.f,    1.f, 1.f,  0.f,
        0.001f, 0.999f, 0.001f,     -1.f, 0.f, 0.f,    0.f, 1.f,  0.f,

        // Face 2 (right)
        // Triangle 1
        0.001f, 0.001f, 0.999f,     0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        0.999f, 0.001f, 0.999f,     0.f, 0.f, 1.f,     1.f, 0.f,  0.f,
        0.999f, 0.999f, 0.999f,     0.f, 0.f, 1.f,     1.f, 1.f,  0.f,
        // Triangle 2
        0.001f, 0.001f, 0.999f,     0.f, 0.f, 1.f,     0.f, 0.f,  0.f,
        0.999f, 0.999f, 0.999f,     0.f, 0.f, 1.f,     1.f, 1.f,  0.f,
        0.001f, 0.999f, 0.999f,     0.f, 0.f, 1.f,     0.f, 1.f,  0.f,

        // Face 3 (back)
        // Triangle 1
        0.999f, 0.001f, 0.999f,     1.f, 0.f, 0.f,     1.f, 0.f,  0.f,
        0.999f, 0.001f, 0.001f,     1.f, 0.f, 0.f,     0.f, 0.f,  0.f,
        0.999f, 0.999f, 0.001f,     1.f, 0.f, 0.f,     0.f, 1.f,  0.f,
        // Triangle 2
        0.999f, 0.001f, 0.999f,     1.f, 0.f, 0.f,     1.f, 0.f,  0.f,
        0.999f, 0.999f, 0.001f,     1.f, 0.f, 0.f,     0.f, 1.f,  0.f,
        0.999f, 0.999f, 0.999f,     1.f, 0.f, 0.f,     1.f, 1.f,  0.f,

        // Face 4 (left)
        // Triangle 1
        0.999f, 0.001f, 0.001f,     0.f, 0.f, -1.f,    1.f, 0.f,  0.f,
        0.001f, 0.001f, 0.001f,     0.001f, 0.f, -1.f,    0.f, 0.f,  0.f,
        0.001f, 0.999f, 0.001f,     0.f, 0.f, -1.f,    0.f, 1.f,  0.f,
        // Triangle 2
        0.999f, 0.001f, 0.001f,     0.f, 0.f, -1.f,    1.f, 0.f,  0.f,
        0.001f, 0.999f, 0.001f,     0.f, 0.f, -1.f,    0.f, 1.f,  0.f,
        0.999f, 0.999f, 0.001f,     0.f, 0.f, -1.f,    1.f, 1.f,  0.f,

        // Face 5 (top)
        // Triangle 1
        0.001f, 0.999f, 0.001f,     0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        0.001f, 0.999f, 0.999f,     0.f, 1.f, 0.f,     1.f, 0.f,  1.f,
        0.999f, 0.999f, 0.999f,     0.f, 1.f, 0.f,     1.f, 1.f,  1.f,
        // Triangle 2
        0.001f, 0.999f, 0.001f,     0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        0.999f, 0.999f, 0.999f,     0.f, 1.f, 0.f,     1.f, 1.f,  1.f,
        0.999f, 0.999f, 0.001f,     0.f, 1.f, 0.f,     0.f, 1.f,  1.f,

        // Face 6 (bottom)
        // Triangle 1
        0.999f, 0.001f, 0.001f,     0.f, -1.f, 0.f,    0.f, 1.f,  2.f,
        0.999f, 0.001f, 0.999f,     0.f, -1.f, 0.f,    1.f, 1.f,  2.f,
        0.001f, 0.001f, 0.999f,     0.f, -1.f, 0.f,    1.f, 0.f,  2.f,
        // Triangle 2
        0.999f, 0.001f, 0.001f,     0.f, -1.f, 0.f,    0.f, 1.f,  2.f,
        0.001f, 0.001f, 0.999f,     0.f, -1.f, 0.f,    1.f, 0.f,  2.f,
        0.001f, 0.001f, 0.001f,     0.001f, -1.f, 0.f,    0.f, 0.f,  2.f
    };

    return generate_vao(data, sizeof(data));
}

GLuint opengl_demo::generate_quad_vao()
{
    float data[] = {
        // Vertices          // Normals         // UVs     // Face type (0: side, 1: top, 2: bottom)

        // Face 1 (top)
        // Triangle 1
        0.f, 0.9375f, 0.f,   0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        0.f, 0.9375f, 1.f,   0.f, 1.f, 0.f,     1.f, 0.f,  1.f,
        1.f, 0.9375f, 1.f,   0.f, 1.f, 0.f,     1.f, 1.f,  1.f,
        // Triangle 2
        0.f, 0.9375f, 0.f,   0.f, 1.f, 0.f,     0.f, 0.f,  1.f,
        1.f, 0.9375f, 1.f,   0.f, 1.f, 0.f,     1.f, 1.f,  1.f,
        1.f, 0.9377f, 0.f,   0.f, 1.f, 0.f,     0.f, 1.f,  1.f
    };

    return generate_vao(data, sizeof(data));
}

GLuint opengl_demo::generate_screen_vao()
{
    float data[] = {
        // Vertices          // UVs
        // Triangle 1
        -1.f, -1.f, 0.f,     0.f, 0.f,
        -1.f, 1.f,  0.f,     0.f, 1.f,
        1.f,  1.f,  0.f,     1.f, 1.f,
        // Triangle 2                 
        -1.f, -1.f, 0.f,     0.f, 0.f,
        1.f,  1.f,  0.f,     1.f, 1.f,
        1.f,  -1.f, 0.f,     1.f, 0.f,
    };

    GLuint my_vao, my_vbo;
    glGenVertexArrays(1, &my_vao);
    glGenBuffers(1, &my_vbo);

    glBindVertexArray(my_vao);
      glBindBuffer(GL_ARRAY_BUFFER, my_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, NULL);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 3));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return my_vao;
}
