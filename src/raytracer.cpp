// Ray Tracer Assignment Starting Code
// Winter 2021

// The JSON library allows you to reference JSON arrays like C++ vectors and JSON objects like C++ maps.

#include "raytracer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "schema.h"
#include "json2scene.h"

const char* PATH = "scenes/";

double fov = 60;
colour3 background_colour(0, 0, 0);

json jscene;
Scene scene;

/****************************************************************************/

// here are some potentially useful utility functions

json find(json& j, const std::string key, const std::string value) {
    json::iterator it;
    for (it = j.begin(); it != j.end(); ++it) {
        if (it->find(key) != it->end()) {
            if ((*it)[key] == value) {
                return *it;
            }
        }
    }
    return json();
}

glm::vec3 vector_to_vec3(const std::vector<float>& v) {
    return glm::vec3(v[0], v[1], v[2]);
}

/****************************************************************************/

void choose_scene(char const* fn) {
    if (fn == NULL) {
        std::cout << "Using default input file " << PATH << "c.json\n";
        fn = "c";
    }

    std::cout << "Loading scene " << fn << std::endl;

    std::string fname = PATH + std::string(fn) + ".json";
    std::fstream in(fname);
    if (!in.is_open()) {
        std::cout << "Unable to open scene file " << fname << std::endl;
        exit(EXIT_FAILURE);
    }

    in >> jscene;

    if (json_to_scene(jscene, scene) < 0) {
        std::cout << "Error in scene file " << fname << std::endl;
        exit(EXIT_FAILURE);
    }

    fov = scene.camera.field;
    background_colour = scene.camera.background;
}

bool trace(const point3& e, const point3& s, colour3& colour, bool pick) {
    // TODO: NOTE: This is a demo, not ray tracing code! You will need to replace all of this with your own code...

    // traverse the objects
    for (auto&& object : scene.objects) {
        if (object->type == "sphere") {
            Sphere* sphere = (Sphere*)(object);
            point3& pos = sphere->position;
            point3 p = -(s - e) * pos[2];
            if (glm::length(glm::vec3(p.x - pos[0], p.y - pos[1], 0)) < sphere->radius) {
                Material& material = object->material;
                colour = material.diffuse;
                // This is NOT correct: it finds the first hit, not the closest
                return true;
            }
        }
    }

    return false;
}