#pragma once

#include <NullGameEngine.hpp>
#include <GameObject.hpp>

namespace null {

    class Camera final : public GameObject {
        public:
            // for now it's ok to have distance as a public member
            // later it could be implemented more prettier with
            // methods that manipulate how the camera moves
            int distance;
    };

}

