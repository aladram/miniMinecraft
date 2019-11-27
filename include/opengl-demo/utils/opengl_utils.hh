#pragma once

#include <iostream>

// Macro du TP 3
#define TEST_OPENGL_ERROR() \
  do { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl; \
  } while(0)
