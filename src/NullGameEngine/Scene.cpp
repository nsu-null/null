#include <memory>

#include <Scene.hpp>

namespace null {

    // box2dWorld is initiated under assumption that
    // gravity is default
    Scene::Scene() : box2dWorld(b2Vec2(0.0, 9.8f)) { }

    void Scene::objectTreeForEachDo(GameObject& gameObject,
            std::function<void(GameObject&)> function) const {

        std::function<void(GameObject&)> walk;
        walk = [&function, &walk](GameObject& go) -> void {
            function(go);
            for (const auto& childWeakRef: go.getChildren()) {
                auto childP = childWeakRef.lock();
                walk(*childP);
            }
        };

        walk(gameObject);
    }

    void Scene::sceneTreeForEachDo(std::function<void(GameObject&)> function) const {
        for (const auto& obj : rootGameObjects) {
            objectTreeForEachDo(*obj, function);
        }
    }

    std::weak_ptr<GameObject> Scene::findFirstByTag(const std::string& tag) {
        std::weak_ptr<GameObject> res;
        bool found = false;
        sceneTreeForEachDo([&tag, &res, &found](GameObject& obj) -> void {
            if (found) {
                return;
            }
            if (obj.hasTag(tag)) {
                res = obj.weak_from_this();
                found = true;
            }
        });
        if (res.lock() == nullptr) {
            throw GameObjectNotFoundException();
        }
        return res;
    }

    std::vector<std::weak_ptr<GameObject>> Scene::findAllByTag(const std::string& tag) {
        std::vector<std::weak_ptr<GameObject>> res;
        sceneTreeForEachDo([&tag, &res](GameObject& obj) -> void {
            if (obj.hasTag(tag)) {
                res.emplace_back(obj.weak_from_this());
            }
        });
        return res;
    }

    std::weak_ptr<GameObject> Scene::addRootGameObject(std::shared_ptr<GameObject>&& newGameObject) {
        newGameObject->scene = weak_from_this();
        rootGameObjects.push_back(newGameObject);

        return newGameObject;
    }

    void Scene::start() {
        camera.start();
        sceneTreeForEachDo([](GameObject& obj) -> void {
                obj.start();
                });

    }

    void Scene::update() {
        camera.update();
        constexpr float timeStep = 1.0f / 60.0f;
        constexpr int velocityIterations = 8;
        constexpr int positionIterations = 3;

        box2dWorld.Step(timeStep, velocityIterations, positionIterations);
        
        sceneTreeForEachDo([](GameObject& obj) -> void {
                obj.update();
                });
    }

    b2World& Scene::getBox2dWorld() {
        return box2dWorld;
    }

    serial::Scene Scene::prefabSerialize() {
        return {};
    }

}

