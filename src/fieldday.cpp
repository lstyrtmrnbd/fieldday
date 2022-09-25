/**
 * fieldday
 * relax and have fun
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <numeric>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using std::cout, std::endl, std::string, std::vector;
using std::ifstream, std::stringstream;
using std::to_string, std::transform_reduce;
using glm::vec2, glm::vec3, glm::mat4;
using glm::rotate, glm::scale, glm::translate;
using glm::radians;

const string VERTFILE = "src/simple.vert";
const string FRAGFILE = "src/simple.frag";

// By default, counterclockwise polygons are taken to be front-facing
vector<vec3> VERTS = {{-1.0f, 1.0f, 0.0f },
                      {-1.0f,-1.0f, 0.0f },
                      { 1.0f,-1.0f, 0.0f },

                      { 1.0f,-1.0f, 0.0f },
                      { 1.0f, 1.0f, 0.0f },
                      {-1.0f, 1.0f, 0.0f }};

vector<vec2> VTEXS = {{ 0.0f, 1.0f },
                      { 0.0f, 0.0f },
                      { 1.0f, 0.0f },

                      { 1.0f, 0.0f },
                      { 1.0f, 1.0f },
                      { 0.0f, 1.0f }};

mat4 newModel(vec3 position, vec2 size, float angle) {
  return translate(position) * rotate(radians(angle), vec3{0.0f, 0.0f, 1.0f}) * scale(vec3{size, 1.0f});
}

vector<mat4> models = {
  newModel({-0.5f,-0.5f,0.5f},{0.6f,0.6f},45.0f),
  newModel({0.0f,-0.5f,0.5f},{0.6f,0.6f},33.0f),
  rotate(radians(45.0f), vec3{0.0f, 0.0f, -1.0f})
};

vector<int> texIdxs = {0,1,2,3,4,5,6,7,8,9,10,11};

vector<vector<int>> stage = {
  {1,1,1,1,1},
  {1,0,0,0,1},
  {1,0,0,0,1},
  {1,0,0,0,1},
  {1,1,1,1,1}
};

void screenshot(const RenderWindow& window) {

  const string filename = "screenshots/screenshot" + to_string((int)time(NULL)) + ".png";

  const auto size = window.getSize();
  
  Texture texture;
  texture.create(size.x, size.y);
  texture.update(window);
  if (texture.copyToImage().saveToFile(filename)) {
    std::cout << "screenshot saved to " << filename << std::endl;
  } else {
    std::cout << "screenshot FAILED to save to " << filename << std::endl;
  }
}

auto getWidth = [](const Image& in) -> unsigned int {
  return in.getSize().x;
 };

auto getHeight = [](const Image& in) -> unsigned int {
  return in.getSize().y;
 };

auto greatest = [](unsigned int first, unsigned int second) -> unsigned int {
  return first > second ? first : second;
 };

GLuint fillTextures(const vector<Image>& images) {

  const unsigned int greatestHeight = transform_reduce(images.begin(), images.end(), 0, greatest, getHeight);
  const unsigned int greatestWidth = transform_reduce(images.begin(), images.end(), 0, greatest, getWidth);

  cout << "Greatest height: " + to_string(greatestHeight) << endl;
  cout << "Greatest width: " + to_string(greatestWidth) << endl;
  
  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

  const int layerCount = images.size();
  
  // Allocate storage of max size
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, greatestWidth, greatestHeight, layerCount, 0, GL_RGBA,  GL_UNSIGNED_BYTE, NULL);
      
  for(auto i = 0; i < layerCount; i++) {

    const auto& image = images[i];
    const auto size = image.getSize();

    const unsigned int xoff = greatestWidth - size.x;
    const unsigned int yoff = greatestHeight - size.y;

    cout << "uploading tex of size (" + to_string(size.x) + ", " + to_string(size.y) + ")"
         << " at offset (" + to_string(xoff) + ", " + to_string(yoff) + ")"
         << " to level " + to_string(i) << endl;
    
    // Upload texture to the center of its level
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, xoff, yoff, i, size.x, size.y, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image.getPixelsPtr());
  }

  // glBindTexture(GL_TEXTURE_2D_ARRAY, 0); //cleanup?

  cout << "Succesfully uploaded textures to target " + to_string(texture) << endl;
  
  return texture;
}

// from OpenGL wiki at khronos.org
void GLAPIENTRY debugCallback( GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar* message,
                               const void* userParam ) {
  
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

int main() {

  //cout << glm::to_string(models[0]) << endl;
  
  ContextSettings settings(24, 0, 0, 4, 6, ContextSettings::Attribute::Core); //depth, stencil, AA, major, minor, profile
  RenderWindow window(VideoMode(1280, 720), "fieldday", Style::Default, settings);
  window.setVerticalSyncEnabled(true);
  window.setActive(true);
  
  // GL Initialization
  glewInit();

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(debugCallback, 0);

  cout << glGetString(GL_VERSION) << endl;
  
  glClearColor(0.9f,0.7f,0.3f,1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glFrontFace(GL_CW); //cull front faces

  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

  /** // not really necessary
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  */
  
  vector<Image> images(12, Image());

  for(auto i = 0; i < 12; i++) {
    images[i].loadFromFile("assets/" + to_string(i) + ".png");
  }
  
  GLuint texture = fillTextures(images);

  ifstream vFile;
  stringstream vStream;

  vFile.open(VERTFILE);
  vStream << vFile.rdbuf();
  vFile.close();

  string vertexCode = vStream.str();
  const char* vShaderCode = vertexCode.c_str();

  ifstream fFile;
  stringstream fStream;

  fFile.open(FRAGFILE);
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


  GLuint instanceVAO, billVBO, texcoordVBO, modelsVBO, texIdxVBO;
  glGenVertexArrays(1, &instanceVAO);
  
  glGenBuffers(1, &billVBO);
  glGenBuffers(1, &texcoordVBO);
  glGenBuffers(1, &modelsVBO);
  glGenBuffers(1, &texIdxVBO);
  
  glBindVertexArray(instanceVAO);

  GLint posLoc = glGetAttribLocation(shader, "position");
  const int posDim = 3; //vec3
  
  glEnableVertexAttribArray(posLoc);
  glBindBuffer(GL_ARRAY_BUFFER, billVBO);///////
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * posDim * VERTS.size(), VERTS.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(posLoc, posDim, GL_FLOAT, false, 0, (GLvoid*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);/////////////

  GLint texcLoc = glGetAttribLocation(shader, "texCoord");
  const int texcDim = 2; //vec2
  
  glEnableVertexAttribArray(texcLoc);
  glBindBuffer(GL_ARRAY_BUFFER, texcoordVBO);///
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texcDim * VTEXS.size(), VTEXS.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(texcLoc, texcDim, GL_FLOAT, true, 0, (GLvoid*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);/////////////

  GLint texIdxLoc = glGetAttribLocation(shader, "texIndex");

  glEnableVertexAttribArray(texIdxLoc);
  glBindBuffer(GL_ARRAY_BUFFER, texIdxVBO);/////
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * texIdxs.size(), texIdxs.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(texIdxLoc, 1, GL_INT, false, 0, (GLvoid*)0);
  glVertexAttribDivisor(texIdxLoc, 1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);/////////////
  
  size_t mat4Stride = sizeof(GLfloat) * 4 * 4;
  
  GLint modelsLoc = glGetAttribLocation(shader, "model");
  glEnableVertexAttribArray(modelsLoc);
  glEnableVertexAttribArray(modelsLoc + 1);
  glEnableVertexAttribArray(modelsLoc + 2);
  glEnableVertexAttribArray(modelsLoc + 3);

  glBindBuffer(GL_ARRAY_BUFFER, modelsVBO);/////
  glBufferData(GL_ARRAY_BUFFER, mat4Stride * models.size(), models.data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(modelsLoc + 0, 4, GL_FLOAT, false, mat4Stride, (GLvoid*)0);
  glVertexAttribPointer(modelsLoc + 1, 4, GL_FLOAT, false, mat4Stride, (GLvoid*)(sizeof(GLfloat) * 4));
  glVertexAttribPointer(modelsLoc + 2, 4, GL_FLOAT, false, mat4Stride, (GLvoid*)(sizeof(GLfloat) * 8));
  glVertexAttribPointer(modelsLoc + 3, 4, GL_FLOAT, false, mat4Stride, (GLvoid*)(sizeof(GLfloat) * 12));

  glVertexAttribDivisor(modelsLoc + 0, 1);
  glVertexAttribDivisor(modelsLoc + 1, 1);
  glVertexAttribDivisor(modelsLoc + 2, 1);
  glVertexAttribDivisor(modelsLoc + 3, 1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);/////////////
  
  glBindVertexArray(0);


  mat4 projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
  mat4 view = glm::lookAt(vec3{0.0f,0.0f,3.0f}, vec3{0.0f}, vec3{0.0f,1.0f,0.0f});
  mat4 viewProjection = projection * view;
  
  GLuint viewProjectionLoc = glGetUniformLocation(shader, "viewProjection");

  GLint texsLoc = glGetUniformLocation(shader, "texs");

  cout << "Shader handle: " + to_string(shader) << endl;
  
  while (window.isOpen()) {
    
    Event event;
    
    while (window.pollEvent(event)) {
      
      if (event.type == Event::Closed)
        window.close();

      if (event.type == Event::KeyPressed &&
          (event.key.code == Keyboard::Escape || event.key.code == Keyboard::Q))
        window.close();

      if(event.type == Event::KeyPressed &&
         (event.key.control && event.key.code == Keyboard::P))
        screenshot(window);
    }

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    
    glUniform1i(texsLoc, 0); // 0 = GL_TEXTURE0
    glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, &viewProjection[0][0]);

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture); // redundant?

    glBindVertexArray(instanceVAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 3);
    glBindVertexArray(0);

    glUseProgram(0);
    
    window.display();
  }
  
  return 0;
}


