#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "tiny_obj_loader.h"
#include <glm.hpp>
#include <iostream>
#include <vector>
#include <string>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Mesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    Material material;
};

bool LoadObj(const std::string& path, Mesh& mesh) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    if (!ret) {
        return false;
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            mesh.vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            mesh.vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            mesh.vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
            mesh.normals.push_back(attrib.normals[3 * index.normal_index + 0]);
            mesh.normals.push_back(attrib.normals[3 * index.normal_index + 1]);
            mesh.normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            mesh.texCoords.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
            mesh.texCoords.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
        }
    }

    if (!materials.empty()) {
        tinyobj::material_t mat = materials[0];
        mesh.material.ambient = glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
        mesh.material.diffuse = glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
        mesh.material.specular = glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
        mesh.material.shininess = mat.shininess;
    }

    return true;
}

#endif

