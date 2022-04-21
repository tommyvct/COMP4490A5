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

bool trace(const point3& eye, const point3& screen, colour3& colour, bool pick)
{
    // TODO: NOTE: This is a demo, not ray tracing code! You will need to replace all of this with your own code...
    
    // TODO: rank by distance, select the shortest one
    // TODO: if pick, print more info

    auto d = screen - eye;
    // traverse the objects
    for (auto&& object : scene.objects)
    {
        if (object->type == "sphere")
        {
            Sphere* sphere = (Sphere*)(object);
            point3& centre = sphere->position;
            
            auto eye2centre = eye - centre;
            auto discriminant = pow(glm::dot(d, eye2centre), 2) - glm::dot(d, d) * (glm::dot(eye2centre, eye2centre) - sphere->radius * sphere->radius);
            
            if (discriminant < -epsilon)  // negative, no intersection
            {
                continue;
            }
            else if ((discriminant) < epsilon)  // zero, one intersection
            {
                auto intersectionT = (glm::dot(-d, eye2centre) + sqrt(discriminant)) / glm::dot(d, d);
                continue;
            }
            else  // positive, 2 intersections
            {
                auto intersectionT1 = (glm::dot(-d, eye2centre) + sqrt(discriminant)) / glm::dot(d, d);
                auto intersectionT2 = (glm::dot(-d, eye2centre) + sqrt(discriminant)) / glm::dot(d, d);
                // TODO: which one is closer?
                continue;
            }
//            point3 p = -(screen - eye) * pos[2];
//            if (glm::length(glm::vec3(p.x - pos[0], p.y - pos[1], 0)) < sphere->radius)
            

        }
        else if (object->type == "plane")
        {
            Plane* plane = (Plane*)(object);
            point3& position = plane->position;
            point3& normal = plane->normal;
            
            auto test = glm::dot(normal, d);
            if (!zero(abs(test)))
            {
                auto intersectionT = glm::dot(normal, (position - eye)) / test;
                if (test < 0)  // front side
                {
                    
                }
                else // (test > 0) back side
                {
                    
                }
            }
        }
        else if (object->type == "triangle")
        {
            Triangle* triangle = (Triangle*)(object);
            Vertex& a = triangle->vertices[0];
            Vertex& b = triangle->vertices[1];
            Vertex& c = triangle->vertices[2];
            
            auto normal = glm::cross((c - b), (b - a));
            auto trianglePlaneTest = glm::dot(normal, a);
            if (abs(trianglePlaneTest) < epsilon)
            {
                continue;
            }
            auto intersectionT = glm::dot(normal, (a - eye)) / trianglePlaneTest;
            auto intersection = eye + intersectionT * d;
            std::vector<double> barycentric =
            {
                glm::dot(glm::cross((b-a), (intersection - a)), normal),
                glm::dot(glm::cross((c-b), (intersection - b)), normal),
                glm::dot(glm::cross((a-c), (intersection - c)), normal)
            };
            
            if
            (!(
                !zero(barycentric[0]) &
                !zero(barycentric[1]) &
                !zero(barycentric[2]) &
                !(
                    barycentric[0] < 0 ^
                    barycentric[1] < 0 ^
                    barycentric[2] < 0
                )
            ))
            {
                continue;
            }
            // TODO: found the intersection

        }
        else if (object->type == "mesh")
        {
            
        }
        
        // TODO: calculate illumination here
        Material& material = object->material;
        colour = material.diffuse;
    }

    return false;
}
