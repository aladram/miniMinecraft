#include <opengl-demo/rendering/renderer.hh>

#include <glad/glad.h>

#include <opengl-demo/vertex_attributes.hh>
#include <opengl-demo/world/block.hh>

using namespace opengl_demo;

block_renderer::block_renderer(GLuint _vao)
    : vao{_vao}
{
    // Generate block positions VBO for world VAO
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(ATTRIB_BLOCKS_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, position));
        glVertexAttribPointer(ATTRIB_BLOCKS_TEXTURES_IDS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, texture_ids));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_POSITIONS);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_TEXTURES_IDS);
      glVertexAttribDivisor(ATTRIB_BLOCKS_POSITIONS, 1);
      glVertexAttribDivisor(ATTRIB_BLOCKS_TEXTURES_IDS, 1);
    glBindVertexArray(0);
}

void block_renderer::render(const std::vector<gl_block>& blocks, bool cached) const
{
    glBindVertexArray(vao);
      if (!cached)
      {
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gl_block) * blocks.size(), blocks.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      }
      glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, blocks.size());
    glBindVertexArray(0);
}
