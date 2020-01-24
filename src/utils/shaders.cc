#include <opengl-demo/utils/shaders.hh>

#include <cassert>
#include <stdexcept>
#include <utility>
#include <glad/glad.h>

#include <opengl-demo/utils/opengl_utils.hh>

template <auto F1, auto F2>
static std::string generic_error(GLint id)
{
    GLint log_size = 0;
    (*F1)(id, GL_INFO_LOG_LENGTH, &log_size);

    std::string p_log(log_size, '\0');
    (*F2)(id, log_size, &log_size, p_log.data());

    return p_log;
}

static std::string compile_error(GLint id)
{
    return generic_error<&glGetShaderiv, &glGetShaderInfoLog>(id);
}

static std::string linking_error(GLint p_id)
{
    return generic_error<&glGetProgramiv, &glGetProgramInfoLog>(p_id);
}

static auto compile_shader(const char* shader_src, GLenum shader_type)
{
    auto id = glCreateShader(shader_type);
    TEST_OPENGL_ERROR();
    if (!id)
        throw std::runtime_error("glCreateShader failed");

    glShaderSource(id, 1, reinterpret_cast<const GLchar**>(&shader_src), nullptr);
    TEST_OPENGL_ERROR();

    glCompileShader(id);
    TEST_OPENGL_ERROR();

    GLint status = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (!status)
        throw std::runtime_error("Compile failed: " + compile_error(id));

    return id;
}

GLuint opengl_demo::compile_shaders(
        const char* vertex_shader_src,
        const char* geometry_shader_src,
        const char* fragment_shader_src
    )
{
    assert(vertex_shader_src);
    assert(fragment_shader_src);

    auto p_id = glCreateProgram();
    TEST_OPENGL_ERROR();
    if (!p_id)
        throw std::runtime_error("glCreateProgram failed");

    auto v_id = compile_shader(vertex_shader_src, GL_VERTEX_SHADER);
    glAttachShader(p_id, v_id);
    TEST_OPENGL_ERROR();

    if (geometry_shader_src)
    {
        auto g_id = compile_shader(geometry_shader_src, GL_GEOMETRY_SHADER);
        glAttachShader(p_id, g_id);
        TEST_OPENGL_ERROR();
    }

    auto f_id = compile_shader(fragment_shader_src, GL_FRAGMENT_SHADER);
    glAttachShader(p_id, f_id);
    TEST_OPENGL_ERROR();

    glLinkProgram(p_id);
    TEST_OPENGL_ERROR();

    GLint status = GL_FALSE;
    glGetProgramiv(p_id, GL_LINK_STATUS, &status);
    if (!status)
        throw std::runtime_error("Linking failed: " + linking_error(p_id));

    return p_id;
}
