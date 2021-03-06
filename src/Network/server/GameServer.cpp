#include <plog/Log.h>
#include "server/GameServer.h"
#include "utils/NetMessageTransmitting.h"
#include "serialized/serverConfig.pb.h"

GameServer::GameServer(std::function<void(void)>& simulation) :
        NetClientCollector(std::move(simulation)) {}

uint8_t GameServer::globalGameID = 1;


void GameServer::broadcastMessage(const net::GameMessage& message) {
    for (int i = 0; i < clientCount(); i++) {
        null::Network::Utils::sendGameMessage(getClient(i), message);
    }
}

void GameServer::handleNetMessage(int clientIdx, const net::NetMessage& message) {
    if (!message.has_game_message()) {
        throw std::invalid_argument("This net message must contain game message");
    }
    handleGameMessage(clientIdx, message.game_message());
}


/**
 * Process game message
 * It is required that message is a command from a client
 * @param clientIdx is ignored for there is only one session right now
 * @param message
 */
void GameServer::handleGameMessage(int clientIdx, const net::GameMessage& message) {
    // it is supposed that server can only receive commands from clients
    auto& commandMessage = message.client_command();
    // distribute
    distributeMessageToSubscribers(commandMessage);
}

void GameServer::distributeMessageToSubscribers(const net::GameMessage::ClientCommand& subscriberState) {
    uint64_t entityId = subscriberState.subscriber_id();
    // add to queue of such subscriber
    LOGD << "Try to distribute message for entity with id " << entityId;
    entityIdToMessageQueue[entityId].push(subscriberState);
}

void GameServer::acceptNewClient() {
    NetClientCollector::acceptNewClient();
    clientIDs.emplace_back(globalGameID++);
}

void GameServer::disconnectClient(int idx) {
    NetClientCollector::disconnectClient(idx);
    if (idx != clientIDs.size() - 1) {
        std::swap(clientIDs[idx], clientIDs.back());
    }
    clientIDs.resize(clientIDs.size() - 1);
}

uint32_t GameServer::clientCount() {
    assert(clients.size() == clientIDs.size());
    return clients.size();
}

uint8_t GameServer::getGameID(int clientIdx) {
    return clientIDs[clientIdx];
}

std::queue<net::GameMessage::ClientCommand>& GameServer::subscribe(uint64_t entityId) {
    return entityIdToMessageQueue[entityId];
}

void GameServer::broadcastMessage(const net::GameMessage::SubscriberState& message) {
    net::GameMessage gameMessage;
    *gameMessage.mutable_subscriber_state() = message;
    broadcastMessage(gameMessage);
}