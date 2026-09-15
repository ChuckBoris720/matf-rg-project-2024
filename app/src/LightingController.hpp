#ifndef MATF_RG_PROJECT_LIGHTINGCONTROLLER_HPP
#define MATF_RG_PROJECT_LIGHTINGCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

struct DirectionalLight {
    glm::vec3 direction{-0.5f, -1.0f, -0.3f};
    glm::vec3 color{1.0f, 0.9f, 0.7f};
    float intensity{1.0f};
};

struct PointLight {
    glm::vec3 position{1.0f, 2.5f, -5.0f};
    glm::vec3 color{2.5f, 0.9f, 0.2f};
    float intensity{2.5f};
};

struct SceneLights {
    DirectionalLight directional;
    PointLight point;
    float ambient_intensity{0.3f};
    glm::vec3 skybox_tint{1.0f, 1.0f, 1.0f};
};

class LightingController : public engine::core::Controller {
public:
    SceneLights lights;
};

#endif
