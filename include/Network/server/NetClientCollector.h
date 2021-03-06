#pragma once

#include <SFML/Network.hpp>
#include <cstdint>
#include <list>
#include "serialized/serverConfig.pb.h"

class NetClientCollector {
private:

    sf::TcpListener listener;

public:
    sf::Thread jobThread;

    sf::IpAddress ipAddress;
    volatile bool threadIsActive;

    static void defaultJob(NetClientCollector* self);

    std::vector<std::unique_ptr<sf::TcpSocket>> clients;
public:

    NetClientCollector();

    int getFirstReadySocketIdx();

    sf::TcpSocket& getClient(int idx);

    virtual void acceptNewClient();

    virtual void handleNetMessage(int clientIdx, const net::NetMessage& message) = 0;

    NetClientCollector(std::function<void()> jobThread);

    virtual ~NetClientCollector();

    void launch();

    uint32_t getIP() const;

    uint16_t getPort() const;

    virtual void disconnectClient(int idx);

    void listen(const std::vector<uint16_t>& ports);

    void listen(uint16_t port);

    static bool isListener(int idx);

};
