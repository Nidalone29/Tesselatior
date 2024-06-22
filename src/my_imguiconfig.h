#ifndef MY_IMGUICONFIG
#define MY_IMGUICONFIG

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#define IM_VEC2_CLASS_EXTRA                                \
  constexpr ImVec2(const glm::vec2& f) : x(f.x), y(f.y) {} \
  operator glm::vec2() const {                             \
    return glm::vec2(x, y);                                \
  }

#endif  // MY_IMGUICONFIG
