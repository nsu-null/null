//
// Created by artemonchelus on 28.05.22.
//

#include "PlayerProgress/HealthBar.hpp"
#include "GameObject.hpp"

namespace null {
    void HealthBar::start() {
        Component::start();
        initialRect = gameObject.getSprite().getTextureRect();
        initialPosition = gameObject.getSprite().getPosition();
    }

    void HealthBar::update() {
        gameObject.setPosition(initialPosition, true);
        Component::update();
    }

    void HealthBar::serialize(google::protobuf::Message& message) const {
        Component::serialize(message);
    }

    HealthBar::HealthBar(GameObject& object) : Component(object) {

    }

    void HealthBar::setProgress(float progress) {
        auto currentRect = gameObject.getSprite().getTextureRect();
        gameObject.getSprite().setTextureRect(sf::IntRect{currentRect.left,
                                                          currentRect.top,
                                                          int(initialRect.width * progress),
                                                          currentRect.height});
    }


}