#pragma once

#include <vector>

#include "gltypes.hpp"

using std::vector;

template<typename T>
void bufferVecs(unsigned int location, const vector<T>& bufferMe, GLenum hint = GL_STATIC_DRAW) {

  typedef typename T::value_type value_t;
  typedef typename getType<value_t>::type gl_t;
  
  const size_t size = sizeof(gl_t);
  const int dim = T::length();

  const GLenum glEnum = getEnum<value_t>::value;

  glBindBuffer(GL_ARRAY_BUFFER, location);
  glBufferData(GL_ARRAY_BUFFER, size * dim * bufferMe.size(), bufferMe.data(), hint);
  glVertexAttribPointer(location, dim, glEnum, false, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  return;
}

// unsigned int bufferMats() {
  
// }


