#include <server/NetClientCollector.h>
#include <SFML/Network.hpp>
#include <cstdint>
#include <utility>
#include <plog/Log.h>
#include <utils/NetMessageTransmitting.h>
#include "exceptions/NetworkException.h"

void NetClientCollector::defaultJob(NetClientCollector *self) {
    while (self->threadIsActive) {
        int readyClientIdx = self->getFirstReadySocketIdx();
        if (readyClientIdx == -2) {
            continue;
        }
        if (isListener(readyClientIdx)) {
            self->acceptNewClient();
            continue;
        }
        try {
            sf::TcpSocket &client = self->getClient(readyClientIdx);
            net::NetMessage message = null::Network::Utils::receiveNetMessage(client);
            self->handleNetMessage(readyClientIdx, message);
        } catch (const ReceiveException &exception) {
            auto status = exception.getStatus();
            if (status == sf::Socket::Disconnected) {
                self->disconnectClient(readyClientIdx);
                continue;
            }
            throw ReceiveException("Unexpected client receive exception status", exception.getStatus());
        }
    }
}

NetClientCollector::NetClientCollector(std::function<void()> jobThread) :
        listener(),
        ipAddress(),
        threadIsActive(true),
        jobThread(std::move(jobThread)) {}

NetClientCollector::NetClientCollector() :
        NetClientCollector([this]() { defaultJob(this); }) {}

NetClientCollector::~NetClientCollector() {
    this->threadIsActive = false;
    jobThread.wait();
    for (int i = 0; i < clients.size();) {
        disconnectClient(i);
    }
}

void NetClientCollector::acceptNewClient() {
    clients.emplace_back(std::make_unique<sf::TcpSocket>());
    sf::TcpSocket &newClient = *clients.back();
    if (listener.accept(newClient) != sf::Socket::Done) {
        LOGD << "Cannot accept new client";
        throw NetworkException("Cannot accept new client");
    }
    LOGD << "New client was accepted";
}

void NetClientCollector::disconnectClient(int idx) {
    if (idx < 0 || idx >= clients.size()) {
        throw std::invalid_argument("Client index is out of range");
    }
    sf::TcpSocket &client = *clients[idx];
    client.disconnect();
    if (idx != clients.size() - 1) {
        clients[idx] = std::move(clients.back());
    }
    clients.resize(clients.size() - 1);
}

/**
 * @return index of first ready socket (-1 means that it is socket listener)
 */
int NetClientCollector::getFirstReadySocketIdx() {
    // TODO not allow one socket to take the whole CPU time
//    LOGD << "Wait for the first ready socket";
    sf::SocketSelector socketSelector;

    for (auto &client: clients) {
        socketSelector.add(*client);
    }
    socketSelector.add(listener);
    auto status = socketSelector.wait(sf::microseconds(1));
    if (!status) {
        return -2;
    }
    if (socketSelector.isReady(listener)) {
        return -1;
    }
    for (int idx = 0; idx < clients.size(); idx++) {
        if (socketSelector.isReady(*clients[idx])) {
            return idx;
        }
    }
    throw NetworkException("Unexpected socket selector case");
}

void NetClientCollector::listen(const std::vector<uint16_t>& ports) {
    for (const auto port: ports) {
        auto status = listener.listen(port, sf::IpAddress::Any);
        if (status == sf::Socket::Done) {
            LOGD << "Successfully bind: " << ipAddress.toString() << " " << port;
            return;
        }
        throw ReceiveException("Cannot bind to socket", status);
    }
}

void NetClientCollector::listen(uint16_t port) {
    listen(std::vector({port}));
}

void NetClientCollector::launch() {
    jobThread.launch();
    LOGD << "New NetClientCollector was launched for new incoming connections";
}

uint32_t NetClientCollector::getIP() const {
    return ipAddress.toInteger();
}

uint16_t NetClientCollector::getPort() const {
    return listener.getLocalPort();
}

bool NetClientCollector::isListener(int idx) {
    return idx == -1;
}

sf::TcpSocket &NetClientCollector::getClient(int idx) {
    return *(clients[idx]);
}
