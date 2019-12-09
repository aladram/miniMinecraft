#pragma once

#include <iostream>

// Macro du TP 3
#define TEST_OPENGL_ERROR() \
  do { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL error " << err << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
  } while(0)
