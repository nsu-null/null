#include "Physics/ContactListener.hpp"
#include "Weapons/WeaponAmmunition/BulletScript.hpp"
#include "GameObjectSensor.hpp"
#include "Weapons/WeaponAmmunition/GrenadeScript.hpp"

namespace null {
//TODO: Make chain call pattern
    void ContactListener::BeginContact(b2Contact* contact) {
        b2ContactListener::BeginContact(contact);
        for (b2Contact* edge = contact; edge; edge = edge->GetNext()) {
            auto go1 = reinterpret_cast<GameObject*>(edge->GetFixtureA()->GetBody()->GetUserData().pointer);
            auto go2 = reinterpret_cast<GameObject*>(edge->GetFixtureB()->GetBody()->GetUserData().pointer);
            for (auto go: std::vector{go1, go2}) {
                // Todo solve the problem with same code for different scripts
                if (go->getScript<BulletScript>() && edge->IsTouching()) {
                    auto otherGo = go == go1 ? go2 : go1;
                    go->getScript<BulletScript>()->contactedGameObject = otherGo;
                }
                if (go->getScript<GrenadeScript>() && edge->IsTouching()) {
                    auto otherGo = go == go1 ? go2 : go1;
                    go->getScript<GrenadeScript>()->contactedGameObject = otherGo;
                }

                // This block of code for describing behavior of Player and Sensor Go
                if (go->getScript<GameObjectSensor>() && edge->IsTouching()) {
                    auto otherGo = go == go1 ? go2 : go1;
                    go->getScript<GameObjectSensor>()->onTouch(otherGo);
                }
            }
        }
    }


    void ContactListener::EndContact(b2Contact* contact) {
        b2ContactListener::EndContact(contact);
    }

    void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
        b2ContactListener::PreSolve(contact, oldManifold);
    }

    void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
        b2ContactListener::PostSolve(contact, impulse);
    }

}
