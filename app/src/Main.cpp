#include <engine/core/Engine.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <spdlog/spdlog.h>

#include "LightingController.hpp"
#include "GUIController.hpp"

class SceneController : public engine::core::Controller {

public:
    void initialize() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void poll_events() override;

private:
    bool m_rotation_mode{false};

    enum class EventState {
        Idle,
        WaitingForSunset,
        WaitingForNight
    };

    EventState m_event_state{EventState::Idle};
    float m_event_timer{0.0f};
};

void SceneController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    camera->Position = glm::vec3(0.0f, 2.0f, 5.0f);
}

void SceneController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KEY_F1).state() == engine::platform::Key::State::JustPressed) { m_rotation_mode = !m_rotation_mode; }

    if (platform->key(engine::platform::KEY_F2).state() == engine::platform::Key::State::JustPressed && m_event_state == EventState::Idle) {
        m_event_state = EventState::WaitingForSunset;
        m_event_timer = 0.0f;
    }
}

void SceneController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    auto lighting = engine::core::Controller::get<LightingController>();

    float dt = platform->dt();

    if (!m_rotation_mode) {
        if (platform->key(engine::platform::KEY_W).is_down()) camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);

        if (platform->key(engine::platform::KEY_A).is_down()) camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);

        if (platform->key(engine::platform::KEY_S).is_down()) camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);

        if (platform->key(engine::platform::KEY_D).is_down()) camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    } else {
        const float rotation_speed = 60.0f;

        if (platform->key(engine::platform::KEY_A).is_down()) camera->rotate_camera(-rotation_speed * dt, 0.0f);

        if (platform->key(engine::platform::KEY_D).is_down()) camera->rotate_camera(rotation_speed * dt, 0.0f);

        if (platform->key(engine::platform::KEY_W).is_down()) camera->rotate_camera(0.0f, rotation_speed * dt);

        if (platform->key(engine::platform::KEY_S).is_down()) camera->rotate_camera(0.0f, -rotation_speed * dt);
    }

    const float vertical_speed = camera->MovementSpeed * dt;

    if (platform->key(engine::platform::KEY_Q).is_down()) camera->Position.y += vertical_speed;

    if (platform->key(engine::platform::KEY_E).is_down()) camera->Position.y -= vertical_speed;

    if (m_event_state != EventState::Idle) {
        m_event_timer += dt;

        // dogadjaj 1 - zalazak sunca
        if (m_event_state == EventState::WaitingForSunset && m_event_timer >= 3.0f) {
            lighting->lights.directional.intensity = 0.5f;
            lighting->lights.directional.color = glm::vec3(1.0f, 0.45f, 0.15f);

            lighting->lights.skybox_tint = glm::vec3(1.0f, 0.55f, 0.35f);

            m_event_state = EventState::WaitingForNight;
            m_event_timer = 0.0f;
        }
        // dogadjaj 2 - noc
        else if (m_event_state == EventState::WaitingForNight && m_event_timer >= 5.0f) {
            lighting->lights.point.intensity = 4.0f;
            lighting->lights.point.color = glm::vec3(1.0f, 0.2f, 0.05f);

            lighting->lights.skybox_tint = glm::vec3(0.18f, 0.22f, 0.4f);

            m_event_state = EventState::Idle;
            m_event_timer = 0.0f;
        }
    }

}

void SceneController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void SceneController::draw() {
    auto *resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto *graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto *lighting = engine::core::Controller::get<LightingController>();

    auto pyramid = resources->model("pyramid");
    auto cactus = resources->model("cactus");
    auto palm = resources->model("palm");
    auto ground = resources->model("ground");

    auto shader = resources->shader("basic");
    auto skybox_shader = resources->shader("skybox");
    auto skybox = resources->skybox("desert");

    auto camera = graphics->camera();
    auto view = camera->view_matrix();
    auto projection = graphics->projection_matrix();

    auto draw_model = [&](engine::resources::Model *model, const glm::mat4 &model_matrix, const glm::vec3 &object_color) {
        shader->use();

        shader->set_bool("useTexture", model == pyramid);

        shader->set_mat4("model", model_matrix);
        shader->set_mat4("view", view);
        shader->set_mat4("projection", projection);

        shader->set_vec3("objectColor", object_color);
        shader->set_vec3("lightDirection", lighting->lights.directional.direction);
        shader->set_vec3("lightColor", lighting->lights.directional.color);
        shader->set_float("directionalIntensity", lighting->lights.directional.intensity);

        shader->set_vec3("pointLightPosition", lighting->lights.point.position);
        shader->set_vec3("pointLightColor", lighting->lights.point.color);
        shader->set_float("pointLightIntensity", lighting->lights.point.intensity);

        shader->set_float("ambientIntensity", lighting->lights.ambient_intensity);

        shader->set_vec3("viewPos", camera->Position);

        model->draw(shader);
    };

    draw_model(
            ground,
            glm::mat4(1.0f),
            glm::vec3(0.55f, 0.0f, 0.15f)
            );

    draw_model(
            pyramid,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(0.0f, 0.0f, -6.0f)
                            ),
                    glm::vec3(0.5f)
                    ),
            glm::vec3(0.8f, 0.55f, 0.25f)
            );

    // Kaktus 1
    draw_model(
            cactus,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(-3.5f, 0.24f, -6.5f)
                            ),
                    glm::vec3(0.8f)
                    ),
            glm::vec3(0.15f, 0.45f, 0.12f)
            );

    // Kaktus 2
    draw_model(
            cactus,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(3.0f, 0.18f, -7.0f)
                            ),
                    glm::vec3(0.65f)
                    ),
            glm::vec3(0.15f, 0.45f, 0.12f)
            );

    // Kaktus 3
    draw_model(
            cactus,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(-5.0f, 0.24f, -10.0f)
                            ),
                    glm::vec3(0.9f)
                    ),
            glm::vec3(0.15f, 0.45f, 0.12f)
            );

    // Palma 1
    draw_model(
            palm,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(4.0f, 0.0f, -9.0f)
                            ),
                    glm::vec3(0.02f)
                    ),
            glm::vec3(0.25f, 0.45f, 0.12f)
            );

    // Palma 2
    draw_model(
            palm,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(6.0f, 0.0f, -12.0f)
                            ),
                    glm::vec3(0.03f)
                    ),
            glm::vec3(0.25f, 0.45f, 0.12f)
            );

    skybox_shader->use();
    skybox_shader->set_vec3("skyboxTint", lighting->lights.skybox_tint);

    graphics->draw_skybox(skybox_shader, skybox);
}

void SceneController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }

class MyApp : public engine::core::App {

public:
    void app_setup() override;
};

void MyApp::app_setup() {
    spdlog::info("Hello, setup!");

    auto scene_controller = register_controller<SceneController>();
    scene_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());

    auto lighting_controller = register_controller<LightingController>();
    lighting_controller->after(scene_controller);

    auto gui_controller = register_controller<GUIController>();
    gui_controller->after(lighting_controller);
}

int main(int argc, char **argv) { return std::make_unique<MyApp>()->run(argc, argv); }
