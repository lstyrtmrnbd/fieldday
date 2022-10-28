

auto mover = [](move& m) -> vec3 {
  const float speed = 0.1f;
  const float horiz = m.left && m.right ? 0.f : m.left ? speed * -1.f : m.right ? speed : 0.f;
  const float vert = m.forward && m.back ? 0.f : m.forward ? speed * -1.f : m.back ? speed : 0.f;
  return vec3{horiz, 0.f, vert};
 };

auto phyziks(const queu& qin, const queue& qout) {

  auto fps = 30;
  auto frametime = 0;
  
  return [qin, qout](state prev_state) {

    frametime += 1;
    
    auto& [move_me, mousedelta] = receiveInput(in_queue);

    current_state cur;
    
    return cur;
  };
}
