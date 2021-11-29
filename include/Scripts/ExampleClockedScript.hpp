#pragma once 

#include <Script.hpp>
#include <GameObject.hpp>
#include <Scripts.hpp>
#include <ClockedScript.hpp>

namespace null {

    class ExampleClockedScript : public ClockedScript {
        public:

        void clockedStart() override;
        void clockedUpdate() override;

        explicit ExampleClockedScript(GameObject& gameObject);

    };
}
