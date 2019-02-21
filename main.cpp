#include <iostream>
#include "GameBoy.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(640, 576), "GameBoy Emulator");
    GameBoy gb (window);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }
        gb.executeFrame();
        window.display();
    }
    return 0;
}
