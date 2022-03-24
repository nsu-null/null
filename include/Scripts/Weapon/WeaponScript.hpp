#pragma once

#include<Script.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <random>
#include <Script.hpp>

using namespace std::chrono_literals;
namespace null {
    class WeaponScript : public Script {
        using mills = std::chrono::milliseconds;
        using clock = std::chrono::steady_clock;
        using time_point = std::chrono::time_point<clock>;

    private:
        mills restartTime = 100ms;
        time_point lastShot;

        std::random_device rd{};
        std::mt19937 gen{rd()};
        std::normal_distribution<> d{0, 0};

    public:
        explicit WeaponScript(GameObject& object);

        virtual void shoot(sf::Vector2f from, sf::Vector2f to) = 0;

        bool checkIfCanShoot();
        void saveShotInfo();

        void serialize(google::protobuf::Message *message) { };
        static std::unique_ptr<WeaponScript> deserialize(google::protobuf::Message *message) { };
    };
};
