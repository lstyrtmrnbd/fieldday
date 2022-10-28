#include <cstdio>
#include <iostream>
#include <functional>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>
#include <ctime>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "shader.hpp"
#include "buffer.hpp"
#include "common_forms.hpp"

using namespace sf;

using std::cout, std::endl;
using std::ifstream, std::stringstream;
using std::string, std::vector, std::function;
using std::to_string, std::transform_reduce, std::ranges::views::iota;

using glm::vec2, glm::vec3, glm::vec4, glm::mat4;
using glm::rotate, glm::scale, glm::translate;
using glm::radians;


struct rend_description {
  Window* window
  shader,
  texture,
  arrays
};

struct rend_type_request {
  shader,
  bufferDescrs, // map buffers to shader
  buffers,
  textureDescrs,
  textures,
  renderingProc // phiziks output goes in here
};

rend_type_request requestMyType();

rend_description initRend(const rend_type_request& type_request);

// recurs
void render(const rend_description& rend_me);
