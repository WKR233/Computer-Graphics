#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include "mesh.h"
#include "shader.h"
#include "../texture_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Model {
public:
    // model data 
    vector<Mesh> meshes;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path) {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path) {
        // load scenes
        tinyobj::ObjReaderConfig reader_config;

        std::string base_dir;
        std::vector<char> seps = { '/','\\' };
        int sep_idx = 0;
        for (; sep_idx < seps.size(); ++sep_idx) {
            size_t idx = path.rfind(seps[sep_idx]);
            if (idx != std::string::npos) {
                base_dir = path.substr(0, idx);
                break;
            }
        }
        std::cout << "TinyObjReader: set base dir = " << base_dir << std::endl;
        reader_config.mtl_search_path = base_dir; // Path to material files
        reader_config.triangulate = true;

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            return;
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        const tinyobj::attrib_t &attrib = reader.GetAttrib();                          // vertex
        const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();              // objects
        const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();     // materials

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces (polygon)
            vector<Vertex> vertices;

            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
                assert(fv == 3);
                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    Vertex vertex;
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    vertex.Position = glm::vec3(vx, vy, vz);

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        vertex.Normal = glm::vec3(nx, ny, nz);
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        vertex.TexCoords = glm::vec2(tx, ty);
                    }
                    vertices.push_back(vertex);
                    ++index_offset;
                }

                // per-face material
                // shapes[s].mesh.material_ids[f];
                // TODO: if you want add texture

                meshes.push_back(Mesh(vertices));
            }
        }
    }
};

#endif