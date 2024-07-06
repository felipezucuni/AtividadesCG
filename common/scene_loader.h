#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <json.hpp>
#include <glm.hpp>

using json = nlohmann::json;

struct SceneConfig {
    std::vector<std::string> objFiles;
    std::vector<glm::vec3> initialTransforms;
    std::vector<glm::vec3> lightPositions;
    glm::vec3 cameraPosition;
    glm::vec3 cameraOrientation;
    float frustum[6];
};

bool LoadSceneConfig(const std::string& path, SceneConfig& config) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    json j;
    file >> j;

    for (const auto& obj : j["objects"]) {
        config.objFiles.push_back(obj["file"]);
        config.initialTransforms.push_back(glm::vec3(obj["transform"]["x"], obj["transform"]["y"], obj["transform"]["z"]));
    }

    for (const auto& light : j["lights"]) {
        config.lightPositions.push_back(glm::vec3(light["position"]["x"], light["position"]["y"], light["position"]["z"]));
    }

    config.cameraPosition = glm::vec3(j["camera"]["position"]["x"], j["camera"]["position"]["y"], j["camera"]["position"]["z"]);
    config.cameraOrientation = glm::vec3(j["camera"]["orientation"]["x"], j["camera"]["orientation"]["y"], j["camera"]["orientation"]["z"]);
    for (int i = 0; i < 6; ++i) {
        config.frustum[i] = j["camera"]["frustum"][i];
    }

    return true;
}

#endif
