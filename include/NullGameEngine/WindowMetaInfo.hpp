#pragma once

#include <SFML/Graphics.hpp>


namespace null {
    class WindowMetaInfo {
    public:
        sf::Vector2u windowsSize = {0, 0};
        sf::Vector2f absoluteMouseWorldCoords = {0, 0};
    };
}


