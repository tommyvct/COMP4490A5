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
 * \param illumination_calculation_recursive_depth illumination recursion counter, 0 is the base case, any negative value will skip illumination calculation
 * \return a pair of\code bool\endcode and\code glm::vec3\endcode object representing calculated illumination colour.
 */
std::pair<bool, colour3> trace(const point3& from, const point3& to, bool pick, int illumination_calculation_recursive_depth)
{
    if (illumination_calculation_recursive_depth == 0)
    {
        return std::make_pair(false, RGB());
    }

    // TODO: if pick, print more info

    Object* best = nullptr;
    float best_t = 99999999.0f;
    glm::vec3 best_normal;

    const auto d = to - from;
    float cufoff = epsilon;

    // intersection
    #if _OPENMP > 201811
    #pragma omp parallel for
    #endif
    for (auto&& object : scene.objects)
    {
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
                intersection_t = std::min(intersection_t1, intersection_t2);
            }

            if (intersection_t < cufoff || intersection_t > best_t)
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
            point3& normal = glm::normalize(plane->normal);
            
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

                if (intersection_t < cufoff || intersection_t > best_t)
                {
                    continue;
                }

                best = object;
                best_t = intersection_t;
                best_normal = normal;
            }
        }
        else if (object->type == "triangle")
        {
            auto triangle = (Triangle*)(object);
            Vertex& a = triangle->vertices[0];
            Vertex& b = triangle->vertices[1];
            Vertex& c = triangle->vertices[2];
            
            auto normal = glm::normalize(glm::cross((c - b), (b - a)));
            auto triangle_plane_test = glm::dot(normal, a);
            if (abs(triangle_plane_test) < epsilon)
            {
                continue;
            }
            auto intersection_t = glm::dot(normal, (a - from)) / triangle_plane_test;
            auto intersection = from + intersection_t * d;
            std::vector<double> barycentric =
            {
                glm::dot(glm::cross((b-a), (intersection - a)), normal),
                glm::dot(glm::cross((c-b), (intersection - b)), normal),
                glm::dot(glm::cross((a-c), (intersection - c)), normal)
            };
            
            if
            (
                !zero(barycentric[0]) &&
                !zero(barycentric[1]) &&
                !zero(barycentric[2]) &&
                (
                    barycentric[0] < 0 &&
                    barycentric[1] < 0 &&
                    barycentric[2] < 0 
                )
            )
            {
                if (intersection_t < cufoff || intersection_t > best_t)
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
            // TODO: mesh intersection test
        }
    }


    // illumination
    if (illumination_calculation_recursive_depth < 0)
    {
        return std::make_pair(best != nullptr, RGB());
    }

    auto best_intersection = from + d * best_t;
    RGB out_colour;
    if (best != nullptr)
    {
        bool out_colour_lock = false;
        // TODO: calculate illumination here
        Material& material = best->material;

        #if _OPENMP > 201811
        #pragma omp parallel for
        #endif
        for (auto&& light : scene.lights)
        {
            enum _light_type { point, spot, directional } light_type;
            if (light->type == "ambient")
            {
                while(out_colour_lock) {}  // NOLINT(bugprone-infinite-loop)
                out_colour_lock = true;
                out_colour.r += light->color.r * best->material.ambient.r;
                out_colour.g += light->color.g * best->material.ambient.g;
                out_colour.b += light->color.b * best->material.ambient.b;
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
                ray_direction = glm::normalize(best_intersection - ((PointLight*) light)->position);
                break;
            case spot:
                ray_direction = glm::normalize(best_intersection - ((SpotLight*) light)->position);
                break;
            case directional:
                ray_direction = glm::normalize(-((DirectionalLight*) light)->direction);
                break;
            }

            if (light_type == point && !std::get<0>(trace(best_intersection, ray_direction, false, -1)) ||
                light_type == spot && acos(glm::dot(ray_direction, glm::normalize(((SpotLight*) light)->direction))) < glm::radians(((SpotLight*) light)->cutoff))
            {
                continue;
            }

            auto diffuse = std::max(glm::dot(best_normal, ray_direction), 0.0f);
            auto r = glm::reflect(-ray_direction, best_normal);
            auto v = glm::normalize(-d);
            auto specular = pow(std::max(glm::dot(r, v), 0.0f), best->material.shininess);

            while(out_colour_lock) {}  // NOLINT(bugprone-infinite-loop)
            out_colour_lock = true;
            out_colour.r += light->color.r * best->material.diffuse.r * diffuse;
            out_colour.g += light->color.g * best->material.diffuse.g * diffuse;
            out_colour.b += light->color.b * best->material.diffuse.b * diffuse;
            out_colour.r += light->color.r * best->material.specular.r * specular;
            out_colour.g += light->color.g * best->material.specular.g * specular;
            out_colour.b += light->color.b * best->material.specular.b * specular;
            out_colour_lock = false;
        }


        // TODO: transmissive and refraction
        auto normalize_d = glm::normalize(d);
        auto normalize_reflect = glm::normalize(glm::reflect(normalize_d, best_normal));
        auto normalize_refract = glm::normalize(glm::refract(normalize_d, best_normal, best->material.refraction));
        if (pick)
        {
            using std::cout;
            using std::endl;
            cout << "d = (" << normalize_d.x << ", " << normalize_d.y << ", " << normalize_d.z << ")" << endl;
            cout << "N = (" << best_normal.x << ", " << best_normal.y << ", " << best_normal.z << ")" << endl;
            cout << "reflect = (" << normalize_reflect.x << ", " << normalize_reflect.y << ", " << normalize_reflect.z << ")" << endl;
            cout << "refract = (" << normalize_refract.x << ", " << normalize_refract.y << ", " << normalize_refract.z << ") with refraction index of " << best->material.refraction << endl;
        }
        best_intersection.x += best_normal.x * 100*epsilon;
        best_intersection.y += best_normal.y * 100*epsilon;
        best_intersection.z += best_normal.z * 100*epsilon;
        auto reflection = trace(best_intersection, normalize_reflect, false, illumination_calculation_recursive_depth - 1);
        auto refraction = trace(best_intersection, normalize_refract, false, illumination_calculation_recursive_depth - 1);
        if (pick && std::get<0>(refraction))
        {
            std::cout << "refraction worked" << std::endl;
        }
        if (pick && std::get<0>(reflection))
        {
            std::cout << "reflection worked" << std::endl;
        }
        if (std::get<0>(reflection))
        {
            out_colour.r += best->material.reflective.r * std::get<1>(reflection).r;
            out_colour.g += best->material.reflective.g * std::get<1>(reflection).g;
            out_colour.b += best->material.reflective.b * std::get<1>(reflection).b;
        }
        if (std::get<0>(refraction))
        {
            //out_colour.r += best->material.refraction.r * std::get<1>(refraction).r;
            //out_colour.g += best->material.refraction.g * std::get<1>(refraction).g;
            //out_colour.b += best->material.refraction.b * std::get<1>(refraction).b;
        }

    }

    return std::make_pair(best != nullptr, out_colour);
}
