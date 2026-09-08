#include <engine/core/Engine.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

class SceneController : public engine::core::Controller {

public:
    void begin_draw() override;

    void draw() override;

    void end_draw() override;
};

void SceneController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void SceneController::draw() {
    auto *resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto pyramid = resources->model("pyramid");
    auto shader = resources->shader("basic");

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
