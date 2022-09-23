/**
 * fieldday
 * relax and have fun
 */

#include <iostream>
#include <functional>
#include <string>
#include <vector>

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
  return scale(vec3{size, 1.0f}) * rotate(radians(angle), vec3{0.0f, 0.0f, 1.0f}) * translate(position);
}

vector<mat4> models = {
  scale(vec3{1.0f, 1.0f, 1.0f}),
  newModel({0.0f,0.0f,0.5f},{0.6f,0.6f},33.0f),
  rotate(radians(45.0f), vec3{0.0f, 0.0f, -1.0f})
};

int main() {

  //cout << glm::to_string(models[0]) << endl;
  
  ContextSettings settings(24, 0, 0, 4, 6); //depth, stencil, AA, major, minor
  RenderWindow window(VideoMode(1280, 720), "fieldday", Style::Default, settings);
  window.setVerticalSyncEnabled(true);
  window.setActive(true);
  
  //GL prep
  glewInit();

  glClearColor(0.9f,0.7f,0.3f,1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glFrontFace(GL_CW); //cull front faces
  
  Texture gremlin;
  if (!gremlin.loadFromFile("gremlin.png")) {
    cout << "Failed to load gremlin.png...\n";
  }

  GLuint texHandle = gremlin.getNativeHandle();

  Shader shader;

  if (!shader.loadFromFile(VERTFILE, FRAGFILE)) {
    std::cout << "Failed to load shader " << VERTFILE
              << ", " << FRAGFILE << std::endl;
  }

  GLuint shaderHandle = shader.getNativeHandle();
  GLuint instanceVAO, billVBO, texcoordVBO, modelsVBO;
  glGenVertexArrays(1, &instanceVAO);
  
  glGenBuffers(1, &billVBO);
  glGenBuffers(1, &texcoordVBO);
  glGenBuffers(1, &modelsVBO);
  
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

  // example buffer update
  //glBufferSubData(GL_ARRAY_BUFFER, offset, size, (GLvoid*)&VERTS[0]);

  
  while (window.isOpen()) {
    
    Event event;
    
    while (window.pollEvent(event)) {
      
      if (event.type == Event::Closed)
        window.close();

      if (event.type == Event::KeyPressed &&
          (event.key.code == Keyboard::Escape || event.key.code == Keyboard::Q))
        window.close();
    }

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader::bind(&shader);

    glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, &viewProjection[0][0]);
    
    glBindVertexArray(instanceVAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 3);
    glBindVertexArray(0);
    
    window.display();
  }
  
  return 0;
}


