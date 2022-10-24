/**
 * fieldday
 * relax and have fun
 */

#include <iostream>
#include <functional>
#include <numeric>
#include <ranges>
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

#include "shader.hpp"
#include "buffer.hpp"

using namespace sf;

using std::cout, std::endl;
using std::ifstream, std::stringstream;
using std::string, std::vector, std::function;
using std::to_string, std::transform_reduce, std::ranges::views::iota;

using glm::vec2, glm::vec3, glm::vec4, glm::mat4;
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

// must sort NEAREST to FARTHEST
vector<mat4> models = {
  rotate(radians(45.0f), vec3{0.0f, 0.0f, -1.0f}),
  newModel({-0.5f,-0.5f,0.5f},{0.6f,0.6f},45.0f),
  newModel({0.0f,-0.5f,0.7f},{0.6f,0.6f},33.0f)
};

vector<int> texIdxs = {0,1,2,3,4,5,6,7,8,9,10,11};

vector<vec4> colors = {
  {0.f,1.f,0.f,1.f},
  {1.f,0.f,0.f,1.f},
  {0.f,0.f,1.f,1.f},
};


struct model {
  int texIdx;
  vec3 position;
  vec2 size;
  float angle;
};

// vector<function<model, model>> modelers

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

    const unsigned int xoff = (greatestWidth - size.x) / 2;
    const unsigned int yoff = (greatestHeight - size.y) / 2;

    cout << "uploading tex of size (" + to_string(size.x) + ", " + to_string(size.y) + ")"
         << " at offset (" + to_string(xoff) + ", " + to_string(yoff) + ")"
         << " to level " + to_string(i) << endl;
    
    // Upload texture to the center of its level
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, xoff, yoff, i, size.x, size.y, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image.getPixelsPtr());
  }

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
  //glFrontFace(GL_CW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const int assetCount = 12;
  
  vector<Image> images(assetCount, Image());

  for(auto i : iota(0,assetCount)) {
    images[i].loadFromFile("assets/" + to_string(i) + ".png");
    images[i].flipVertically(); // upside down pngs
  }
  
  GLuint texture = fillTextures(images);

  // texture MUST BE BOUND
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 1.0f); // anisotropic filtering 1-16

  const GLuint shader = compileShader(VERTFILE, FRAGFILE);

  GLuint instanceVAO, billVBO, texcoordVBO, modelsVBO, texIdxVBO, colorsVBO;
  glGenVertexArrays(1, &instanceVAO);
  
  glGenBuffers(1, &billVBO);
  glGenBuffers(1, &texcoordVBO);
  glGenBuffers(1, &modelsVBO);
  glGenBuffers(1, &texIdxVBO);
  glGenBuffers(1, &colorsVBO);

  glBindVertexArray(instanceVAO);

  GLint posLoc = glGetAttribLocation(shader, "position");  
  glEnableVertexAttribArray(posLoc);
  bufferVecs(billVBO, posLoc, VERTS);

  GLint texcLoc = glGetAttribLocation(shader, "texCoord");  
  glEnableVertexAttribArray(texcLoc);
  bufferVecs(texcoordVBO, texcLoc, VTEXS);

  GLint texIdxLoc = glGetAttribLocation(shader, "texIndex");
  glEnableVertexAttribArray(texIdxLoc);
  bufferVecs(texIdxVBO, texIdxLoc, texIdxs, GL_STATIC_DRAW, 1);

  GLint colorsLoc = glGetAttribLocation(shader, "colors");
  glEnableVertexAttribArray(colorsLoc);
  bufferVecs(colorsVBO, colorsLoc, colors, GL_STATIC_DRAW, 1);
    
  GLint modelsLoc = glGetAttribLocation(shader, "model");
  glEnableVertexAttribArray(modelsLoc);
  glEnableVertexAttribArray(modelsLoc + 1);
  glEnableVertexAttribArray(modelsLoc + 2);
  glEnableVertexAttribArray(modelsLoc + 3);
  bufferMats(modelsVBO, modelsLoc, models, GL_STATIC_DRAW, 1);
  
  glBindVertexArray(0);

  Vector2i center(window.getSize().x / 2, window.getSize().y / 2);
  Mouse::setPosition(center, window);
  
  vec3 myTarget = vec3{0.f};
  vec3 myPosition = vec3{0.f,0.f,3.f};

  mat4 projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
  mat4 view = glm::lookAt(myPosition, myTarget, vec3{0.0f,1.0f,0.0f});
  mat4 viewProjection = projection * view;


  GLuint viewProjectionLoc = glGetUniformLocation(shader, "viewProjection");
  GLint texsLoc = glGetUniformLocation(shader, "texs");

  cout << "Shader handle: " + to_string(shader) << endl;

  struct move {
    bool left {false};
    bool right {false};
    bool forward {false};
    bool back {false};
  };

  auto mover = [](move& m) -> vec3 {
    const float speed = 0.1f;
    const float horiz = m.left && m.right ? 0.f : m.left ? speed * -1.f : m.right ? speed : 0.f;
    const float vert = m.forward && m.back ? 0.f : m.forward ? speed * -1.f : m.back ? speed : 0.f;
    return vec3{horiz, 0.f, vert};
  };

  move moveMe;
  
  while (window.isOpen()) {
    
    Event event;
    
    while (window.pollEvent(event)) {
      
      if (event.type == Event::Closed)
        window.close();

      if(event.type == Event::KeyPressed) {

        if(event.key.code == Keyboard::Escape || event.key.code == Keyboard::Q)
          window.close();
        
        if(event.key.control && event.key.code == Keyboard::P)
          screenshot(window);

        if(event.key.code == Keyboard::A || event.key.code == Keyboard::Left)
          moveMe.left = true;

        if(event.key.code == Keyboard::D || event.key.code == Keyboard::Right)
          moveMe.right = true;

        if(event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
          moveMe.forward = true;

        if(event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
          moveMe.back = true;
      }

      if(event.type == Event::KeyReleased) {
        if(event.key.code == Keyboard::A || event.key.code == Keyboard::Left)
          moveMe.left = false;

        if(event.key.code == Keyboard::D || event.key.code == Keyboard::Right)
          moveMe.right = false;

        if(event.key.code == Keyboard::W || event.key.code == Keyboard::Up)
          moveMe.forward = false;

        if(event.key.code == Keyboard::S || event.key.code == Keyboard::Down)
          moveMe.back = false;
      }
    }

    Vector2i delta = Mouse::getPosition(window) - center;
    Mouse::setPosition(center, window);

    myTarget.x += delta.x * 0.01f;
    myTarget.y -= delta.y * 0.01f;

    vec3 movement = mover(moveMe);
    myTarget += movement;
    myPosition += movement;

    vec3 up = vec3{0.0f,1.0f,0.0f};
    
    view = glm::lookAt(myPosition, myTarget, up);
    viewProjection = projection * view;

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture); // redundant?
    
    glUniform1i(texsLoc, 0); // 0 = GL_TEXTURE0
    glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, &viewProjection[0][0]);

    glBindVertexArray(instanceVAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 3);
    glBindVertexArray(0);

    glUseProgram(0);
    
    window.display();
  }
  
  return 0;
}


