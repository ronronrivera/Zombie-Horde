#pragma once

#include "engine/Window.hpp"
#include <functional>

class GameLoop{
public:
    using UpdateFn = std::function<void (float dt)>;
    using RenderFn = std::function<void ()>;

    GameLoop(Window &window, UpdateFn update, RenderFn render);

    void run();
private:
    Window &m_window;
    UpdateFn m_update;
    RenderFn m_render;
    
    static constexpr float FIXED_DT = 1.0 / 60.0f; 
};
