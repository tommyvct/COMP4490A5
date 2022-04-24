#include <glm/glm.hpp>
#include <utility>

typedef glm::vec3 point3;
typedef glm::vec3 colour3;

extern double fov;
extern colour3 background_colour;

void choose_scene(char const *fn);
std::pair<bool, colour3> trace(const point3 &from, const point3 &to, bool pick, int recursive_depth = 4);
