#include <engine/core/Engine.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <spdlog/spdlog.h>

class SceneController : public engine::core::Controller {

public:
    void initialize() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;
};

void SceneController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->Position = glm::vec3(0.0f, 2.0f, 5.0f);
}

void SceneController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).is_down()) camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);

    if (platform->key(engine::platform::KEY_A).is_down()) camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);

    if (platform->key(engine::platform::KEY_S).is_down()) camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);

    if (platform->key(engine::platform::KEY_D).is_down()) camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);


    auto mouse = platform->mouse();

    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void SceneController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void SceneController::draw() {
    auto *resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto *graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto pyramid = resources->model("pyramid");
    auto shader = resources->shader("basic");

    auto camera = graphics->camera();

    auto view = camera->view_matrix();
    auto projection = graphics->projection_matrix();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    shader->use();
    shader->set_mat4("model", model);
    shader->set_mat4("view", view);
    shader->set_mat4("projection", projection);

    pyramid->draw(shader);
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
