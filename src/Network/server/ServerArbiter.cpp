#include "server/ServerArbiter.h"
#include <plog/Log.h>
#include <random>
#include <exception>
#include "utils/util.h"
#include "utils/NetMessageTransmitting.h"

//TODO: what about destruction the game?
std::string ServerArbiter::createNewGameSimulation() {
    PLOGD << "New game simulation is being created";
    srand(time(nullptr));
    uint16_t port = freePorts.back();
    freePorts.pop_back();
    net::GameServerConfig gameServerConfig;
    if (!gameServers.empty()) {
        LOGD << "ERROR multiple servers";
        throw std::length_error("Right now we do not allow multiple servers");
    }
    gameServers.emplace_back(std::make_unique<GameServer>(simulation));
    gameServers.back()->listen(port);
    gameServers.back()->launch();

    std::string roomCode = generateSixLetterCode();
    while (roomCodeToServerNum.contains(roomCode)) {
        roomCode = generateSixLetterCode();
    }

    PLOGD << "Room code was chosen: " << roomCode;
    roomCodeToServerNum[roomCode] = gameServers.size() - 1;
    return roomCode;
}

ServerArbiter::ServerArbiter() : NetClientCollector(),
                                 freePorts({6000, 6001, 6002, 6003, 6004, 6005, 6006, 6007, 6008, 6009, 6010}),
                                 gameServers() {}

ServerArbiter::ServerArbiter(std::function<void()> simulation)
        : NetClientCollector(),
          simulation(std::move(simulation)),
          freePorts({6000, 6001, 6002, 6003, 6004, 6005, 6006, 6007, 6008, 6009, 6010}) {}

void ServerArbiter::sendGameServerConfig(sf::TcpSocket& client, const std::string& roomCode) {
    net::NetMessage message;
    net::GameServerConfig* serverConfig = message.mutable_server_config();
    if (!roomCodeToServerNum.contains(roomCode)) {
        throw std::logic_error("No such room");
//        sendNetMessage(client, message);
    }
    GameServer& server = *(gameServers[roomCodeToServerNum[roomCode]]);
    serverConfig->set_room_code(roomCode);
    serverConfig->set_v4(sf::IpAddress("127.0.0.1").toInteger());
    serverConfig->set_server_port(server.getPort());
    null::Network::Utils::sendNetMessage(client, message);
}

void ServerArbiter::handleNetMessage(int clientIdx, const net::NetMessage& message) {
    switch (message.body_case()) {
        case net::NetMessage::kGenerateRoomRequest: {
            LOGD << "New game generation request received";
            std::string roomCode = createNewGameSimulation();
            sendRoomCode(*clients[clientIdx], roomCode);
            break;
        }
        case net::NetMessage::kConnectRoom: {
            LOGD << "Request about asking getting game server config by server";
            sendGameServerConfig(*clients[clientIdx], message.connect_room().room_code());
            break;
        }
        case net::NetMessage::kGameMessage:
        case net::NetMessage::kClientInfo:

        case net::NetMessage::kServerConfig:
        case net::NetMessage::BODY_NOT_SET:
            break;
    }
}

void ServerArbiter::sendRoomCode(sf::TcpSocket& socket, const std::string& roomCode) {
    LOGD << "sending room code to the " << socket.getRemoteAddress() << " " << socket.getRemotePort();
    net::NetMessage netMessage;
    netMessage.mutable_connect_room()->set_room_code(roomCode);
    null::Network::Utils::sendNetMessage(socket, netMessage);
}

ServerArbiter::~ServerArbiter() {

}

