#include <engine/core/Engine.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <spdlog/spdlog.h>
#include <imgui.h>

class SceneController : public engine::core::Controller {

public:
    void initialize() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void poll_events() override;

private:
    glm::vec3 m_directional_direction{-0.5f, -1.0f, -0.3f};
    glm::vec3 m_directional_color{1.0f, 0.9f, 0.7f};
    float m_directional_intensity{1.0f};

    glm::vec3 m_point_light_position{1.0f, 2.5f, -5.0f};
    glm::vec3 m_point_light_color{2.5f, 0.9f, 0.2f};
    float m_point_light_intensity{2.5f};

    float m_ambient_intensity{0.3f};

    bool m_cursor_enabled{true};
};

void SceneController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    camera->Position = glm::vec3(0.0f, 2.0f, 5.0f);
}

void SceneController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }
}

void SceneController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    if (m_cursor_enabled) return;

    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).is_down())
        camera->move_camera(
                engine::graphics::Camera::Movement::FORWARD, dt);

    if (platform->key(engine::platform::KEY_A).is_down())
        camera->move_camera(
                engine::graphics::Camera::Movement::LEFT, dt);

    if (platform->key(engine::platform::KEY_S).is_down())
        camera->move_camera(
                engine::graphics::Camera::Movement::BACKWARD, dt);

    if (platform->key(engine::platform::KEY_D).is_down())
        camera->move_camera(
                engine::graphics::Camera::Movement::RIGHT, dt);
}

void SceneController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void SceneController::draw() {
    auto *resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto *graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

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

        shader->set_mat4("model", model_matrix);
        shader->set_mat4("view", view);
        shader->set_mat4("projection", projection);

        shader->set_vec3("objectColor", object_color);
        shader->set_vec3("lightDirection", m_directional_direction);
        shader->set_vec3("lightColor", m_directional_color);
        shader->set_float("directionalIntensity", m_directional_intensity);

        shader->set_vec3("pointLightPosition", m_point_light_position);
        shader->set_vec3("pointLightColor", m_point_light_color);
        shader->set_float("pointLightIntensity", m_point_light_intensity);

        shader->set_float("ambientIntensity", m_ambient_intensity);

        shader->set_vec3("viewPos", camera->Position);

        model->draw(shader);
    };

    draw_model(
            ground,
            glm::mat4(1.0f),
            glm::vec3(0.55f, 0.35f, 0.15f)
            );

    draw_model(
            pyramid,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(0.0f, 0.0f, -6.0f)
                            ),
                    glm::vec3(1.5f)
                    ),
            glm::vec3(0.8f, 0.55f, 0.25f)
            );

    // Kaktus 1
    draw_model(
            cactus,
            glm::scale(
                    glm::translate(
                            glm::mat4(1.0f),
                            glm::vec3(-3.5f, 1.0f, -6.5f)
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
                            glm::vec3(3.0f, 0.8f, -7.0f)
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
                            glm::vec3(-5.0f, 1.1f, -10.0f)
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
    graphics->draw_skybox(skybox_shader, skybox);


    graphics->begin_gui();

    ImGui::Begin("Lighting");

    ImGui::Text("Directional light");

    ImGui::DragFloat3(
            "Direction",
            &m_directional_direction.x,
            0.05f,
            -1.0f,
            1.0f
            );

    ImGui::ColorEdit3(
            "Directional color",
            &m_directional_color.x
            );

    ImGui::SliderFloat(
            "Directional intensity",
            &m_directional_intensity,
            0.0f,
            3.0f
            );

    ImGui::Separator();

    ImGui::Text("Point light");

    ImGui::DragFloat3(
            "Position",
            &m_point_light_position.x,
            0.05f
            );

    ImGui::ColorEdit3(
            "Point color",
            &m_point_light_color.x
            );

    ImGui::SliderFloat(
            "Point intensity",
            &m_point_light_intensity,
            0.0f,
            5.0f
            );

    ImGui::Separator();

    ImGui::Text("Ambient");

    ImGui::SliderFloat(
            "Ambient intensity",
            &m_ambient_intensity,
            0.0f,
            1.0f);

    ImGui::End();

    graphics->end_gui();
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
}

int main(int argc, char **argv) { return std::make_unique<MyApp>()->run(argc, argv); }
