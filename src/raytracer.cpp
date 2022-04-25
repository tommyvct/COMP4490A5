// Ray Tracer Assignment Starting Code
// Winter 2021

// The JSON library allows you to reference JSON arrays like C++ vectors and JSON objects like C++ maps.

// ReSharper disable CppCStyleCast
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

/**
 * \brief Single instance of ray trace.
 * \param from the eye location, or secondary intersection points
 * \param to the location on the near side of view frustum, or the direction of secondary reflected ray
 * \param pick true to print more information
 * \param illumination_calculation_recursive_depth illumination recursion counter, 0 is the base case
 * \return a pair of\code bool\endcode and\code glm::vec3\endcode object representing calculated illumination colour.
 */
std::pair<bool, colour3> trace(const point3& from, const point3& to, Object* exclude, bool pick, int illumination_calculation_recursive_depth)
{
    if (illumination_calculation_recursive_depth <= 0)
    {
        return std::make_pair(false, RGB());
    }


    //intersection
    auto intersection_tuple = intersect_next_t(from, to, nullptr, nullptr, pick, epsilon);

    Object* best = std::get<0>(intersection_tuple);
    if (!best)
    {
        return std::make_pair(false, RGB());
    }

    float best_t = std::get<1>(intersection_tuple);
    glm::vec3 best_normal = std::get<2>(intersection_tuple);
    const auto d = to - from;
    auto best_intersection = from + d * best_t + best_normal * 50.0f * epsilon;


    // illumination
    RGB out_colour;
    bool out_colour_lock = false;
    Material& material = best->material;

#if _OPENMP > 201811
#pragma omp parallel for
#endif
    for (auto&& light : scene.lights)
    {
        enum _light_type { point, spot, directional } light_type{};
        if (light->type == "ambient")
        {
            while(out_colour_lock) {}  // NOLINT(bugprone-infinite-loop)
            out_colour_lock = true;
            out_colour += light->color * best->material.ambient;
            out_colour_lock = false;
            continue;
        }

        if (light->type == "point")
        {
            light_type = point;
        }
        else if (light->type == "spot")
        {
            light_type = spot;
        }
        else if (light->type == "directional")
        {
            light_type = directional;
        }

        glm::vec3 ray_direction;

        switch (light_type)
        {
        case point:
            ray_direction = -glm::normalize(best_intersection - ((PointLight*)light)->position);
            break;
        case spot:
            ray_direction = -glm::normalize(best_intersection - ((SpotLight*)light)->position);
            break;
        case directional:
            ray_direction = -glm::normalize(((DirectionalLight*)light)->direction);
            break;
        }

        auto obstacle = intersect_next_t(best_intersection, best_intersection + ray_direction, nullptr, nullptr, false,
                                         0);
        if (std::get<0>(obstacle) != nullptr)
        {
            if (light_type == directional)
            {
                continue;
            }

            auto distance_to_obstacle = abs(glm::length(best_intersection + std::get<1>(obstacle) * (best_intersection + ray_direction)));
            auto distance_to_light = abs(glm::distance(best_intersection, ((SpotLight*)light)->position));
            if (distance_to_obstacle < distance_to_light)
            {
                continue;
            }
        }

        if (light_type == spot && acos(glm::dot(ray_direction, glm::normalize(-((SpotLight*)light)->direction))) > glm::radians(((SpotLight*)light)->cutoff))
        {
            continue;
        }

        auto d_length = glm::length(d);
        auto attenuation = 1 / (1 + 0.1*d_length + 0.2*d_length*d_length);
        float diffuse = std::max(glm::dot(best_normal, ray_direction), 0.0f) * attenuation;
        auto r = glm::reflect(ray_direction, best_normal);
        auto v = glm::normalize(d);
        float specular = pow(std::max(glm::dot(r, v), 0.0f), best->material.shininess) * attenuation;

        while(out_colour_lock) {}  // NOLINT(bugprone-infinite-loop)
        out_colour_lock = true;
        out_colour += light->color * best->material.diffuse * diffuse;
        out_colour += light->color * best->material.specular * specular;
        out_colour_lock = false;
    }

    auto reflect = glm::reflect(glm::normalize(best_intersection - from), best_normal);
    auto reflection = trace(best_intersection, best_intersection + reflect, nullptr, pick, illumination_calculation_recursive_depth - 1);
    bool do_refraction = true;
    if (std::get<0>(reflection))
    {

        auto test = 1 - pow((1/best->material.refraction), 2) * (1 - pow(glm::dot(best_intersection - from, best_normal), 2));
        if (best->material.refraction != 0.0f && test >= 0.0f)
        {
            if (pick)
            {
                std::cout << "full reflection instead of refraction" << std::endl;
            }
            out_colour += std::get<1>(reflection);
            do_refraction = false;
        }
        else
        {
            out_colour += best->material.reflective * std::get<1>(reflection);
        }
    }

    best_intersection -= 100.0f * best_normal * epsilon;

    if (!zero(best->material.refraction) && do_refraction)
    {
        auto refract = glm::refract(glm::normalize(best_intersection - from), best_normal, 1/best->material.refraction);
        auto next_t = intersect_next_t(best_intersection, best_intersection + refract, nullptr, best, pick,
                                       -epsilon);
        if (pick)
        {
            std::cout << "refraction object " << ((std::get<0>(next_t) == best) ? "match" : "mismatch") << std::endl;
        }
        if (std::get<0>(next_t) == best)
        {
            auto refraction_exit = best_intersection + std::get<1>(next_t) * (best_intersection + refract) + std::get<2>(next_t) * 100.0f * epsilon;
            auto refraction = trace(refraction_exit, refraction_exit + glm::normalize(best_intersection - from), best, pick, illumination_calculation_recursive_depth - 1);
            if (pick)
            {
                std::cout << "refraction = ( " << std::get<1>(refraction).r << ", " << std::get<1>(refraction).g << ", " << std::get<1>(refraction).b <<  ")" << std::endl;
            }
            if (std::get<0>(refraction))
            {
                out_colour = ((glm::vec3(1, 1, 1) - best->material.transmissive) * out_colour) + (best->material.transmissive * std::get<1>(refraction));
            }
        }
    }
    else
    {
        if (pick)
        {
            std::cout << "reflection" << std::endl;
        }
        auto transmit = best_intersection + glm::normalize(d) +  best_normal * epsilon;
        auto transmission = trace(best_intersection, transmit, best, pick, illumination_calculation_recursive_depth - 1);
        out_colour = ((glm::vec3(1, 1, 1) - best->material.transmissive) * out_colour) + (best->material.transmissive * std::get<1>(transmission));
    }

    return std::make_pair(true, out_colour);
}

std::tuple<Object *, float, glm::vec3>
intersect_next_t(const point3 &from, const point3 &to, Object *exclude, Object *only, bool pick, float start_t)
{
    Object* best = nullptr;
    float best_t = 99999999.0f;
    glm::vec3 best_normal;

    const auto d = to - from;

    // intersection
#if _OPENMP > 201811
#pragma omp parallel for
#endif
    for (auto&& object : scene.objects)
    {
        if (only && object != only)
        {
            continue;
        }
        if (object == exclude)
        {
            continue;
        }

        if (object->type == "sphere")
        {
            auto sphere = (Sphere*)(object);
            point3& centre = sphere->position;

            auto eye2centre = from - centre;
            auto discriminant = pow(glm::dot(d, eye2centre), 2) - glm::dot(d, d) * (glm::dot(eye2centre, eye2centre) - sphere->radius * sphere->radius);
            float intersection_t = 0;

            if (discriminant < -epsilon)  // negative, no intersection
            {
                continue;
            }
            else if ((discriminant) < epsilon)  // zero, one intersection
            {
                intersection_t = (glm::dot(-d, eye2centre) + sqrt(discriminant)) / glm::dot(d, d);
            }
            else  // positive, 2 intersections
            {
                float intersection_t1 = (glm::dot(-d, eye2centre) + sqrt(discriminant)) / glm::dot(d, d);
                float intersection_t2 = (glm::dot(-d, eye2centre) - sqrt(discriminant)) / glm::dot(d, d);
//                intersection_t1 = intersection_t1 < start_t ? 999999.0f : intersection_t1;
//                intersection_t2 = intersection_t2 < start_t ? 999999.0f : intersection_t2;
                intersection_t = std::min(intersection_t1, intersection_t2);
//                if (intersection_t1 == intersection_t2 && intersection_t1 == 999999.0f)
//                    continue;
            }

            if (intersection_t < start_t || intersection_t > best_t)
            {
                continue;
            }

            best = object;
            best_t = intersection_t;

            auto intersection = from + d * intersection_t;
            best_normal = glm::normalize(intersection - centre);
        }
        else if (object->type == "plane")
        {
            auto plane = (Plane*)(object);
            point3& position = plane->position;
            point3 normal = glm::normalize(plane->normal);

            auto test = glm::dot(normal, d);
            if (!zero(test))
            {
                auto intersection_t = glm::dot(normal, (position - from)) / test;
                //if (test < 0)  // front side
                //{
                //    
                //}
                //else // (test > 0) back side
                //{
                //    
                //}

                if (intersection_t < start_t || intersection_t > best_t)
                {
                    continue;
                }

                best = object;
                best_t = intersection_t;
                best_normal = normal;
            }
        }
        else if (object->type == "mesh")
        {
            auto mesh = (Mesh*)object;

            for (auto&& triangle : mesh->triangles)
            {
                Vertex& a = triangle.vertices[0];
                Vertex& b = triangle.vertices[1];
                Vertex& c = triangle.vertices[2];

                auto normal = glm::normalize(glm::cross((c - b), (b - a)));
                auto triangle_plane_test = glm::dot(normal, d);
                if (abs(triangle_plane_test) < epsilon)
                {
                    continue;
                }
                auto intersection_t = glm::dot(normal, (b - from)) / triangle_plane_test;
                auto intersection = from + intersection_t * d;
                std::vector<double> barycentric =
                {
                    glm::dot(glm::cross((b - a), (intersection - a)), normal),
                    glm::dot(glm::cross((c - b), (intersection - b)), normal),
                    glm::dot(glm::cross((a - c), (intersection - c)), normal)
                };

                if
                (
                        barycentric[0] < -epsilon &&
                        barycentric[1] < -epsilon &&
                        barycentric[2] < -epsilon
                )
                {
                    if (intersection_t < start_t || intersection_t > best_t)
                    {
                        continue;
                    }

                    best = object;
                    best_t = intersection_t;
                    best_normal = -normal;//barycentric[0] < -epsilon ? -normal : normal;
                }
            }
        }
    }

    return std::make_tuple(best, best_t, best_normal);
}