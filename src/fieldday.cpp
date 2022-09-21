/**
 * fieldday
 * relax and have fun
 */

#include <iostream>
#include <functional>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using std::cout, std::string, std::vector;
using glm::vec2, glm::vec3;

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

int main() {

  ContextSettings settings(24, 0, 0, 4, 6); //depth, stencil, AA, major, minor
  RenderWindow window(VideoMode(1280, 720), "fieldday", Style::Default, settings);
  window.setVerticalSyncEnabled(true);
  window.setActive(true);
  
  //GL prep
  glewInit();

  glClearColor(0.3f,0.6f,0.5f,1.0f);
  glEnable(GL_DEPTH_TEST);
  
  Texture gremlin;
  if (!gremlin.loadFromFile("gremlin.png")) {
    cout << "Failed to load gremlin.png...\n";
  }

  GLuint texHandle = gremlin.getNativeHandle();
  
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  Shader shader;

  if (!shader.loadFromFile(VERTFILE, FRAGFILE)) {
    std::cout << "Batch failed to load shader " << VERTFILE
              << ", " << FRAGFILE << std::endl;
  }

  GLuint shaderHandle = shader.getNativeHandle();

  unsigned int posLength = 3, texcLength = 2;
  unsigned long posOffset = 0;
  unsigned long texcOffset = posLength * sizeof(GLfloat);
  unsigned int stride = (posLength * sizeof(GLfloat)) + (texcLength * sizeof(GLfloat));

  GLint posLoc = glGetAttribLocation(shaderHandle, "position");
  glEnableVertexAttribArray(posLoc);
  glVertexAttribPointer(posLoc, posLength, GLfloat, false, stride, (GLvoid*)posOffset);

  GLint texcLoc = glGetAttribLocation(shaderHandle, "texCoords");
  glEnableVertexAttribArray(texcLoc);
  glVertexAttribPointer(texcLoc, texcLength, GLfloat, true, stride, (GLvoid*)texcOffset);

  // clear bindings for cleanliness
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);
  for(auto i; i < VERTS) {
    
  }
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, (GLvoid*)&VERTS[0]);

  
  while (window.isOpen()) {
    
    Event event;
    
    while (window.pollEvent(event)) {
      
      if (event.type == Event::Closed)
        window.close();

      if (event.type == Event::KeyPressed &&
          (event.key.code == Keyboard::Escape || event.key.code == Keyboard::Q))
        window.close();
    }
    
    window.clear();
    // window.draw(sprite);
    window.display();
  }
  
  return 0;
}


