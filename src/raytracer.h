#include <glm/glm.hpp>
#include <utility>

#include "schema.h"

typedef glm::vec3 point3;
typedef glm::vec3 colour3;

extern double fov;
extern colour3 background_colour;

void choose_scene(char const *fn);
std::pair<bool, colour3> trace(const point3 &from, const point3 &to, Object *exclude = nullptr, bool pick = false, int recursive_depth = 6);
std::tuple<Object *, float, glm::vec3> intersect_next_t(const point3 &from, const point3 &to, Object *exclude = nullptr, bool pick = false, float start_t = epsilon);
