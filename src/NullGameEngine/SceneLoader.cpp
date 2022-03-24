#include <memory>

#include <box2d/box2d.h>
        
#include <Scene.hpp>
#include <SceneLoader.hpp>
#include <MainLoop.hpp>
#include <Scripts.hpp>
#include <GameObject.hpp>
#include <ResourceManager.hpp>
#include <PlayerAnimation.hpp>
#include <Utility.hpp>
#include <functional>
#include <unordered_map>
#include "MapManager/MapManager.hpp"
#include "Weapon/WeaponScript.hpp"
#include "Weapon/StraightWeaponScript.hpp"
#include <MapManager/MapManager.hpp>

namespace null {

    // todo this is a dummy implementation
    // later reimplement this by loading stuff from file 
    // and using a resource manager
    void SceneLoader::loadSceneFromFile(std::filesystem::path path) {

        enum Level {
            Demo, Menu
        };

        // a temporary solution to get a scene by keyword
        // while we don't store them in files
        auto keyword = path.string();

        const std::unordered_map<std::string, std::function<std::shared_ptr<Scene>(void)>> keywordToLevelGetter = {
                {"/demo",                 getDemoScene},
                {"/menu",                 getMenuScene},
                {"/menu/play",            getPlayScene},
                {"/menu/play/createRoom", getCreateRoomScene},
                {"/menu/play/joinRoom",   getJoinRoomScene},
                {"/game",                 getGameScene}
        };

        std::shared_ptr<Scene> scene;

        scene = keywordToLevelGetter.at(keyword)();
        MainLoop::provideScene(scene);
    }

    static std::shared_ptr<Scene> getGameScene() {
        return nullptr;
    }

    std::shared_ptr<Scene> SceneLoader::getDemoScene() {
        // todo this should be done in a scene file
        auto newScene = std::make_shared<Scene>();
        auto& box2dWorld = newScene->getBox2dWorld();

        newScene->camera->addScript<ExampleCameraScript>(*newScene->camera);
        // this texture is not released on purpose, because it MUST exist for as long
        // as the sprite lives. todo manage it with resource manager
        sf::Texture* nullTexture = ResourceManager::loadTexture("background.png");

        auto nullGameLogo = std::make_shared<GameObject>();
        nullGameLogo->getSprite().setTexture(*nullTexture);
        nullGameLogo->getSprite().setScale({8.0f, 8.0f});
        nullGameLogo->renderLayer = serial::BACKGROUND;
        nullGameLogo->visible = true;

        auto boxTexture = ResourceManager::loadTexture("box.png");

        auto boxObject = std::make_shared<GameObject>();
        boxObject->getSprite().setTexture(*boxTexture);
        boxObject->getSprite().setScale(0.125f, 0.125f);
        boxObject->setPosition(200, 0);
        boxObject->renderLayer = serial::FOREGROUND;
        boxObject->visible = true;

        auto boxObject2 = std::make_shared<GameObject>();
        boxObject2->getSprite().setTexture(*boxTexture);
        boxObject2->getSprite().setScale(0.125f, 0.125f);
        boxObject2->setPosition(750.0f, 200.0f);
        boxObject2->getSprite().setColor(sf::Color(255U, 0U, 0U));
        boxObject2->renderLayer = serial::BACKGROUND1;
        boxObject2->visible = true;
        auto createGround = [&box2dWorld, &newScene](float x, float y) {
            auto groundObject = std::make_shared<GameObject>();
            auto& groundSprite = groundObject->getSprite();
            groundSprite.setTexture(*ResourceManager::loadTexture("platform.png"));
            groundSprite.setScale(3.0f, 3.0f);
            groundSprite.setPosition(x, y);
            groundObject->renderLayer = serial::FOREGROUND;
            groundObject->visible = true;
            groundObject->makeStatic(box2dWorld);
            groundObject->addScript<ReloadSceneScript>(*groundObject);
            newScene->addRootGameObject(std::move(groundObject));
        };
        createGround(0, 400);
        createGround(192, 466);
        createGround(384, 532);
        createGround(576, 532);
        createGround(1152, 400);
        createGround(880, 100);
        for (int i = 0; i < 10; i++) {
            createGround(i * 300, i * 200);
        }
        boxObject->makeDynamic(box2dWorld);

        auto cursorObject = std::make_shared<GameObject>();

        auto spriteSheet = SpriteSheet("cursorAnim.png", sf::Vector2i(16, 16), {{"cursorAnim", 0, 0, 5}});
        cursorObject->addScript<CursorAnimation>(*cursorObject, spriteSheet);
        cursorObject->getSprite().setScale(4.0f, 4.0f);
        cursorObject->renderLayer = serial::FOREGROUND;
        cursorObject->visible = true;

        auto player = std::make_shared<GameObject>();
        player->getSprite().setTextureRect({0, 0, 30, 54});
        player->getSprite().setScale(3.0f, 3.0f);
//        player->setPosition(300, 300);
        player->visible = true;
        player->renderLayer = serial::FOREGROUND1;
        player->makeDynamic(box2dWorld);
        player->getRigidBody()->SetFixedRotation(true);
        auto playerSpriteSheet = SpriteSheet("playerAnim_v2.png", {30, 54}, {{"idle",      0, 0, 7},
                                                                             {"walkRight", 1, 0, 3},
                                                                             {"walkLeft",  2, 0, 3}});

        auto weapon = std::make_shared<GameObject>();
        weapon->addScript<StraightWeaponScript>(*weapon);
        player->addChild(std::move(weapon));
        newScene->camera->getScript<ExampleCameraScript>()->setTrackedGameObject(*player);
        newScene->camera->getScript<ExampleCameraScript>()->setMap(*nullGameLogo);

        auto shape1 = new b2PolygonShape();
        auto sizeVector = Utility::pixelToMetersVector(sf::Vector2i{60, 162});
        shape1->SetAsBox(sizeVector.x / 2, sizeVector.y / 2, player->getRigidBody()->GetLocalCenter(), 0.0f);
        b2FixtureDef fixtureDef1;
        fixtureDef1.shape = shape1;
        fixtureDef1.density = 1;

        auto shape2 = new b2PolygonShape();
        sizeVector = Utility::pixelToMetersVector(sf::Vector2i{90, 162});
        shape2->SetAsBox(sizeVector.x / 2, sizeVector.y / 2, player->getRigidBody()->GetLocalCenter(), 0.0f);
        b2FixtureDef fixtureDef2;
        fixtureDef2.shape = shape2;
        fixtureDef2.density = 1;

        auto shape3 = new b2PolygonShape();
        sizeVector = Utility::pixelToMetersVector(sf::Vector2i{78, 162});
        shape3->SetAsBox(sizeVector.x / 2, sizeVector.y / 2, player->getRigidBody()->GetWorldCenter(), 0.0f);
        b2FixtureDef fixtureDef3;
        fixtureDef3.shape = shape3;
        fixtureDef3.density = 1;

        player->addScript<PlayerAnimation>(*player, playerSpriteSheet,
                                           std::unordered_map<std::string, std::vector<std::vector<b2FixtureDef>>>{
                                                   {"idle",      {{fixtureDef3}, {fixtureDef3}, {fixtureDef3}, {fixtureDef3}, {fixtureDef3}, {fixtureDef3}, {fixtureDef3}, {fixtureDef3}}},
                                                   {"walkRight", {{fixtureDef1}, {fixtureDef2}, {fixtureDef1}, {fixtureDef2}}},
                                                   {"walkLeft",  {{fixtureDef1}, {fixtureDef2}, {fixtureDef1}, {fixtureDef2}}}
                                           });
        MapManager mapManager(box2dWorld);
        newScene->addRootGameObject(std::move(mapManager.makeBorder(nullGameLogo->getSprite())));
        nullGameLogo->addChild(std::move(boxObject));
        //nullGameLogo->addChild(std::move(boxObject2));
        newScene->addRootGameObject(std::move(nullGameLogo));
        //newScene->addRootGameObject(std::move(cursorObject));
        newScene->addRootGameObject(std::move(player));
        return newScene;
    }

    std::shared_ptr<Scene> SceneLoader::getPlayScene() {
        return nullptr;
    }

    std::shared_ptr<Scene> SceneLoader::getCreateRoomScene() {
        return nullptr;
    }

    std::shared_ptr<Scene> SceneLoader::getJoinRoomScene() {
        return nullptr;
    }

    std::shared_ptr<Scene> SceneLoader::getGameScene() {
        return nullptr;
    }

    std::shared_ptr<Scene> SceneLoader::getMenuScene() {

        auto newScene = std::make_shared<Scene>();
        auto& box2dWorld = newScene->getBox2dWorld();
        sf::Texture* nullTexture = ResourceManager::loadTexture("menu/menu_background.png");

        auto background = std::make_shared<GameObject>();
        background->getSprite().setTexture(*nullTexture);
        background->getSprite().setPosition({0, 0});
        background->renderLayer = serial::BACKGROUND;
        background->visible = true;

        auto cursorObject = std::make_shared<GameObject>(std::set<std::string>({"cursor"}));

        auto spriteSheet = SpriteSheet("cursorAnim.png", sf::Vector2i(16, 16), {{"cursorAnim", 0, 0, 5}});
        cursorObject->addScript<CursorAnimation>(*cursorObject, spriteSheet);
        cursorObject->renderLayer = serial::FOREGROUND3;
        cursorObject->addTag("cursor");
        cursorObject->visible = true;
        cursorObject->getSprite().setScale(0.1f, 0.1f);
        cursorObject->makeDynamic(box2dWorld);
        cursorObject->getSprite().setScale(4.f, 4.f);
        cursorObject->getRigidBody()->GetFixtureList()->SetSensor(true);

        sf::Texture* pressedTexture = ResourceManager::loadTexture("menu/buttons/null_text.png");

        auto playButton = std::make_shared<GameObject>();
        auto playButtonTexture = ResourceManager::loadTexture("menu/buttons/play.png");
        playButton->setPosition(450, 380);
        playButton->addScript<ButtonScript>(*playButton, *playButtonTexture, *pressedTexture, []() -> void {
            SceneLoader::changeScene("/demo");
        });
        playButton->renderLayer = serial::FOREGROUND;
        playButton->visible = true;

        auto exitButton = std::make_shared<GameObject>();
        auto exitButtonTexture = ResourceManager::loadTexture("menu/buttons/exit.png");
        exitButton->setPosition(450, 600);
        exitButton->addScript<ButtonScript>(*exitButton, *exitButtonTexture, *pressedTexture, []() -> void {
            std::exit(0);
        });
        exitButton->renderLayer = serial::FOREGROUND;
        exitButton->visible = true;


        auto optionsButton = std::make_shared<GameObject>();
        auto optionsButtonTexture = ResourceManager::loadTexture("menu/buttons/options.png");
        optionsButton->setPosition(320, 490);
        optionsButton->addScript<ButtonScript>(*optionsButton, *optionsButtonTexture, *pressedTexture,
                                               []() -> void {});
        optionsButton->renderLayer = serial::FOREGROUND;
        optionsButton->visible = true;


        newScene->addRootGameObject(std::move(background));
        newScene->addRootGameObject(std::move(cursorObject));
        newScene->addRootGameObject(std::move(playButton));
        newScene->addRootGameObject(std::move(optionsButton));
        newScene->addRootGameObject(std::move(exitButton));

        return newScene;
    };

    void SceneLoader::changeScene(std::filesystem::path path) {
        loadSceneFromFile(path);
        throw SceneChangedException();
    }

}

