#include <cstddef>

#include <glad/glad.h>

GLuint generate_cube_vao()
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

    GLuint my_vao, my_vbo, my_ebo;
    glGenVertexArrays(1, &my_vao);
    glGenBuffers(1, &my_vbo);
    glGenBuffers(1, &my_ebo);

    glBindVertexArray(my_vao);
      glBindBuffer(GL_ARRAY_BUFFER, my_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (5 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return my_vao;
}
