#pragma once

#include <memory>

#include <box2d/box2d.h>

#include <NullGameEngine.hpp>
#include <GameObject.hpp>
#include <Camera.hpp>

namespace null {
    class Scene : public std::enable_shared_from_this<Scene> {
    private:
        Camera camera;
        std::vector<std::shared_ptr<GameObject>> rootGameObjects;
        b2World box2dWorld;
    public:
        Scene();

        void objectTreeForEachDo(GameObject&,
                std::function<void(GameObject&)>) const; 

        void sceneTreeForEachDo(std::function<void(GameObject&)>) const;

        void start();

        void update();

        std::weak_ptr<GameObject> addRootGameObject(std::shared_ptr<GameObject>&&);

        b2World &getBox2dWorld();

        friend Renderer;

        friend SceneLoader;

        virtual serial::Scene prefabSerialize();

    };
}

