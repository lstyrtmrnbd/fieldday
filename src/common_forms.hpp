#pragma once

// speculative
struct model {
  int texIdx;
  vec3 position;
  vec2 size;
  float angle;
};

struct move {
  bool left {false};
  bool right {false};
  bool forward {false};
  bool back {false};
};

struct phyz_in {
  move mover;
  bool ended {false};
};


