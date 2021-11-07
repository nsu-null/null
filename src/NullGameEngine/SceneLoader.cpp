#include <memory>
        
#include <Scene.hpp>
#include <SceneLoader.hpp>
#include <MainLoop.hpp>

namespace null {

    // todo this is a dummy implementation
    // later reimplement this by loading stuff from file 
    // and using a resource manager
    void SceneLoader::loadSceneFromFile(std::string) {
        
        // todo this should be done in a scene file
        auto newScene = std::make_unique<Scene>();

        // this texture is not released on purpose, because it MUST exist for as long
        // as the sprite lives. todo manage it with resource manager
        sf::Texture* nullTexture = new sf::Texture();
        nullTexture->loadFromFile("../null.jpg");
        auto sprite = std::make_shared<sf::Sprite>(*nullTexture);
        auto nullGameLogo = std::make_unique<GameObject>(sprite);
        newScene->addGameObject(move(nullGameLogo));

        MainLoop::provideScene(move(newScene));
    };

}

