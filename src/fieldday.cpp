/**
 * fieldday
 * relax and have fun
 */

#include <iostream>
#include <functional>
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

vector<int> texIdxs = {0,1,2};

vector<vector<int>> stage = {
  {1,1,1,1,1},
  {1,0,0,0,1},
  {1,0,0,0,1},
  {1,0,0,0,1},
  {1,1,1,1,1}
};

void screenshot(const RenderWindow& window) {

  const string filename = "screenshots/screenshot" + std::to_string((int)time(NULL)) + ".png";

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

GLuint fillTextures(const vector<Image>& images) {

  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
  
  for(auto i = 0; i < images.size(); i++) {

    const auto& image = images[i];
    const auto size = image.getSize();
    
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, size.x, size.y, i, 0, GL_RGBA,  GL_UNSIGNED_BYTE, (GLvoid*)image.getPixelsPtr());
  }

  // glBindTexture(GL_TEXTURE_2D_ARRAY, 0); //cleanup?
  
  return texture;
}

// from OpenGL wiki
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
  
  ContextSettings settings(24, 0, 0, 4, 6); //depth, stencil, AA, major, minor
  RenderWindow window(VideoMode(1280, 720), "fieldday", Style::Default, settings);
  window.setVerticalSyncEnabled(true);
  window.setActive(true);
  
  // GL Initialization
  glewInit();

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(debugCallback, 0);
  
  glClearColor(0.9f,0.7f,0.3f,1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glFrontFace(GL_CW); //cull front faces

  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  
  vector<Image> images(12, Image());

  for(auto i = 0; i < 12; i++) {
    images[i].loadFromFile("assets/" + std::to_string(i) + ".png");
  }
  
  GLuint texture = fillTextures(images);

  Shader shader;

  if (!shader.loadFromFile(VERTFILE, FRAGFILE)) {
    std::cout << "Failed to load shader " << VERTFILE
              << ", " << FRAGFILE << std::endl;
  }

  GLuint shaderHandle = shader.getNativeHandle();
  GLuint instanceVAO, billVBO, texcoordVBO, modelsVBO, texIdxVBO;
  glGenVertexArrays(1, &instanceVAO);
  
  glGenBuffers(1, &billVBO);
  glGenBuffers(1, &texcoordVBO);
  glGenBuffers(1, &modelsVBO);
  glGenBuffers(1, &texIdxVBO);
  
  glBindVertexArray(instanceVAO);

  GLint posLoc = glGetAttribLocation(shaderHandle, "position");
  const int posDim = 3; //vec3
  
  glEnableVertexAttribArray(posLoc);
  glBindBuffer(GL_ARRAY_BUFFER, billVBO);///////
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * posDim * VERTS.size(), VERTS.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(posLoc, posDim, GL_FLOAT, false, 0, (GLvoid*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);/////////////

  GLint texcLoc = glGetAttribLocation(shaderHandle, "texCoord");
  const int texcDim = 2; //vec2
  
  glEnableVertexAttribArray(texcLoc);
  glBindBuffer(GL_ARRAY_BUFFER, texcoordVBO);///
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texcDim * VTEXS.size(), VTEXS.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(texcLoc, texcDim, GL_FLOAT, true, 0, (GLvoid*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);/////////////

  GLint texIdxLoc = glGetAttribLocation(shaderHandle, "texIndex");

  glEnableVertexAttribArray(texIdxLoc);
  glBindBuffer(GL_ARRAY_BUFFER, texIdxVBO);/////
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * texIdxs.size(), texIdxs.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(texIdxLoc, 1, GL_INT, false, 0, (GLvoid*)0);
  glVertexAttribDivisor(texIdxLoc, 1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);/////////////
  
  size_t mat4Stride = sizeof(GLfloat) * 4 * 4;
  
  GLint modelsLoc = glGetAttribLocation(shaderHandle, "model");
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
  
  GLuint viewProjectionLoc = glGetUniformLocation(shaderHandle, "viewProjection");

  GLint texsLoc = glGetUniformLocation(shaderHandle, "texs");


  cout << "Shader handle: " + std::to_string(shaderHandle) << endl;
  
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

    //Shader::bind(&shader); // does texture binding internally
    glUseProgram(shaderHandle);
    
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


