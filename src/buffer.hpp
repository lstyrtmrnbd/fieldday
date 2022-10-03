#pragma once

#include <vector>

#include "gltypes.hpp"

using std::vector;

template<typename T>
void bufferVecs(unsigned int vbo, unsigned int location, const vector<T>& bufferMe, GLenum hint = GL_STATIC_DRAW, int divisor = 0) {

  typedef typename T::value_type value_t;
  // typedef typename getType<value_t>::type gl_t;
  // const size_t size = sizeof(gl_t);
  const size_t size = sizeof(value_t);
  const int dim = T::length();

  const GLenum glEnum = getEnum<value_t>::value;

  const unsigned int bufferSize = size * dim * bufferMe.size();
    
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferMe.data(), hint);
  glVertexAttribPointer(location, dim, glEnum, false, 0, NULL);
  glVertexAttribDivisor(location, divisor);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  return;
}

template<>
void bufferVecs<int>(unsigned int vbo, unsigned int location, const vector<int>& bufferMe, GLenum hint, int divisor) {
  
  const size_t size = sizeof(int);

  const GLenum glEnum = getEnum<int>::value;

  const unsigned int bufferSize = size * bufferMe.size();
    
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferMe.data(), hint);
  glVertexAttribIPointer(location, 1, glEnum, 0, NULL);            // note the 'I'
  glVertexAttribDivisor(location, divisor);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  return;
}

template<typename T>
void bufferMats(unsigned int vbo, unsigned int location, const vector<T>& bufferMe, GLenum hint = GL_STATIC_DRAW, int divisor = 0) {

  typedef typename T::value_type value_t;
  const size_t size = sizeof(value_t);
  const int col_dim = T::length();
  const int row_dim = bufferMe[0].length();
  
  const GLenum glEnum = getEnum<value_t>::value;

  const size_t stride = size * col_dim * row_dim;
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, stride * bufferMe.size(), bufferMe.data(), hint);

  for(auto i = 0; i < col_dim; i++) {
    // row_dim vs col_dim used correctly? doesnt matter for mat4x4... 
    glVertexAttribPointer(location + i, row_dim, glEnum, false, stride, (GLvoid*)(i * size * row_dim));
    glVertexAttribDivisor(location + i, divisor);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return;
}


