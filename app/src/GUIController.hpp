#ifndef MATF_RG_PROJECT_GUICONTROLLER_HPP
#define MATF_RG_PROJECT_GUICONTROLLER_HPP

#include <engine/core/Controller.hpp>

class GUIController : public engine::core::Controller {
public:
    void draw() override;
};

#endif