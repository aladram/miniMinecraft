#include <opengl-demo/my-shaders.hh>

#include <glad/glad.h>
#include <opengl-demo/utils/shaders.hh>

using namespace opengl_demo;

extern const char vertex_shader_src[];
extern const char fragment_shader_src[];
extern const char vertex_2_shader_src[];
extern const char fragment_2_shader_src[];

GLuint opengl_demo::compile_my_shaders()
{
    return compile_shaders(vertex_shader_src, fragment_shader_src);
}

GLuint opengl_demo::compile_my_shaders_2()
{
    return compile_shaders(vertex_2_shader_src, fragment_2_shader_src);
}
