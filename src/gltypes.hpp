#pragma once

#include <GL/glew.h>

template <typename GLtype>
struct getEnum {
  const static GLenum value;
};

template<>
struct getEnum<signed char> {
  const static GLenum value = GL_BYTE;
};

template<>
struct getEnum<unsigned char> {
  const static GLenum value = GL_UNSIGNED_BYTE;
};

template<>
struct getEnum<short int> {
  const static GLenum value = GL_SHORT;
};

template<>
struct getEnum<unsigned short int> {
  const static GLenum value = GL_UNSIGNED_SHORT;
};

template<>
struct getEnum<int> {
  const static GLenum value = GL_INT;
};

template<>
struct getEnum<unsigned int> {
  const static GLenum value = GL_UNSIGNED_INT;
};

template<>
struct getEnum<float> {
  const static GLenum value = GL_FLOAT;
};

template<>
struct getEnum<double> {
  const static GLenum value = GL_DOUBLE;
};

template <typename T>
struct getType {
  typedef T type;
};

template<>
struct getType<float> {
  typedef GLfloat type;
};

template<>
struct getType<int> {
  typedef GLint type;
};


