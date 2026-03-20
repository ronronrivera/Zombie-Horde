#include "engine/Window.hpp"
#include "engine/GameLoop.hpp"
#include <iostream>

int main () {
   
   try {
        
        Window window(1280, 720, "ZombieHorde - Ron-ron Aspe Rivera");

        auto update = [](float dt){
            (void)dt;
        };

        auto render = []() {
            
        };

        GameLoop loop(window, update, render);
        loop.run();

   }
   catch (const std::exception &e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    } 

    return 0;
}

