#include "shader.hpp"

unsigned int compileShader(string vertfile, string fragfile) {
  
  ifstream vFile;
  stringstream vStream;

  vFile.open(vertfile);
  vStream << vFile.rdbuf();
  vFile.close();

  string vertexCode = vStream.str();
  const char* vShaderCode = vertexCode.c_str();

  ifstream fFile;
  stringstream fStream;

  fFile.open(fragfile);
  fStream << fFile.rdbuf();
  fFile.close();

  string fragCode = fStream.str();
  const char* fShaderCode = fragCode.c_str();
  
  GLuint vertex, fragment;
  int success;
  char infoLog[512];
   
  // vertex Shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  
  if(!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    cout << "ERROR: Vertex shader compilation failed\n" << infoLog << endl;
  }

  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  
  if(!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    cout << "ERROR: Fragment shader compilation failed\n" << infoLog << endl;
  }

  // shader Program
  GLuint shader = glCreateProgram();
  glAttachShader(shader, vertex);
  glAttachShader(shader, fragment);
  glLinkProgram(shader);
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  
  if(!success) {
    glGetProgramInfoLog(shader, 512, NULL, infoLog);
    cout << "ERROR: Shader linking failed\n" << infoLog << endl;
  }

  // cleanup
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  glUseProgram(shader);

  return shader;
}
