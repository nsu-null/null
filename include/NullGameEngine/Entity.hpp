#pragma once

#include <cstdint>

namespace null {

    class Entity {
    public:
        uint64_t guid;
        Entity();
        explicit Entity(uint64_t guid);
    };

}
