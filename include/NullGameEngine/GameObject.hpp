#pragma once

#include <memory>
#include <set>

#include <box2d/box2d.h>

#include <SFML/Graphics.hpp>

#include <NullGameEngine.hpp>
#include <Script.hpp>
#include <RenderLayer.hpp>
#include <algorithm>

enum class GameObjectStatus {
    NONE, RUNNING, DEATCHED
};
namespace null {

    class GameObject : public std::enable_shared_from_this<GameObject> {
    private:
        void assertSpriteHasSize();

        void setRigidBodyDefPositionBySprite(b2BodyDef&);

        void setShapeAsBoxBySprite(b2PolygonShape&);

        std::weak_ptr<Scene> scene;
        std::string name;
    public:
        const std::string& getName() const;

        void setName(const std::string& name);

    protected:
        sf::Sprite sprite;
        b2Body* rigidBody = nullptr;
        std::weak_ptr<GameObject> parent;
        std::vector<std::shared_ptr<GameObject>> children;
        std::set<std::string> tags;
        std::vector<std::unique_ptr<Script>> scripts;
        GameObjectStatus gameObjectStatus = GameObjectStatus::NONE;

        void start();

        void update();

    public:

        RenderLayer renderLayer;

        GameObject();

        explicit GameObject(std::set<std::string> tags);

        ~GameObject();

        std::weak_ptr<GameObject> addChild(std::shared_ptr<GameObject>&&);

        std::weak_ptr<GameObject> getParent() const;

        std::weak_ptr<Scene> getScene();

        bool visible;

        sf::Sprite& getSprite();

        b2Body* getRigidBody();

        void makeStatic(b2World& box2dWorld);
        void makeStatic();
        void makeDynamic(b2World& box2dWorld);

        void setCollisionCategories(uint16_t categoryBits);

        void setCollisionMasks(uint16_t maskBits);

        void detachFromPhysicsWorld();

        // Returns the children of this GameObject
        // Potentially is VERY expensive!
        std::vector<std::weak_ptr<GameObject>> getChildren();

        // Returns the child of this GameObject by its index
        std::weak_ptr<GameObject> getChild(int index);

        std::vector<std::unique_ptr<Script>>& getScripts();

        void addTag(const std::string& tag);

        bool removeTag(const std::string& tag);

        bool hasTag(const std::string& tag);

        const sf::Transform& getTransform();

        const sf::Vector2f& getPosition();

        void setPosition(float x, float y);

        void setPosition(const sf::Vector2f& pos);

        void addScript(std::unique_ptr<Script> script);

        template<class T, typename... Args>
        void addScript(Args&& ... args) {
            auto script =
                    std::make_unique<T>(std::forward<Args>(args)...);
            scripts.push_back(std::move(script));
        }

        // Returns a script by a given name (each script must have name)
        template<class T>
        T* getScript() {
            for (auto& script: getScripts()) {

                if (dynamic_cast<T*>(script.get()) != nullptr) {
                    return dynamic_cast<T*>(script.get());
                }
            }
            return nullptr;
        }

        friend Scene;

        void makeDynamic();

        void deleteChild(GameObject* childToDelete);

        void deleteMe();

        GameObject* getCollied();

        Scene& getSceneForce();

        std::shared_ptr<GameObject> findFirstChildrenByTag(const std::string& tag);
    };

}

