#include <stdexcept>
#include <utility>
#include <glad/glad.h>

#include <opengl-demo/utils/opengl_utils.hh>
#include <opengl-demo/utils/shaders.hh>

template <auto F1, auto F2>
static std::string generic_error(GLint id)
{
    GLint log_size = 0;
    (*F1)(id, GL_INFO_LOG_LENGTH, &log_size);

    std::string p_log(log_size, '\0');
    (*F2)(id, log_size, &log_size, p_log.data());

    return std::move(p_log);
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

GLuint opengl_demo::compile_shaders(const char* vertex_shader_src, const char* fragment_shader_src)
{
    auto v_id = compile_shader(vertex_shader_src, GL_VERTEX_SHADER);
    auto f_id = compile_shader(fragment_shader_src, GL_FRAGMENT_SHADER);

    auto p_id = glCreateProgram();
    TEST_OPENGL_ERROR();
    if (!p_id)
        throw std::runtime_error("glCreateProgram failed");

    glAttachShader(p_id, v_id);
    TEST_OPENGL_ERROR();

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
