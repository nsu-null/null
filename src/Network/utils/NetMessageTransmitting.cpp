#include "utils/NetMessageTransmitting.h"
#include "plog/Log.h"
#include <exceptions/NetworkException.h>

namespace null::Network::Utils {
    net::NetMessage receiveNetMessage(sf::TcpSocket& socket) {
        net::NetMessage message;
        sf::Packet packet;
        auto status = socket.receive(packet);
        if (status != sf::Socket::Done) {
            throw ReceiveException("Cannot receive message", status);
        }
        std::string resultString;
        packet >> resultString;
        message.ParseFromString(resultString);
        return message;
    }


    void sendNetMessage(sf::TcpSocket& socket, const net::NetMessage& message) {
        sf::Packet packet;
        packet << message.SerializeAsString();
        auto old = socket.isBlocking();
        socket.setBlocking(true);
        auto status = socket.send(packet);
        socket.setBlocking(old);
        if (status != sf::Socket::Done) {
            LOGD << "Cannot send message";
            throw ReceiveException("Error occurred while trying to send message to client", status);
        }
        LOGD << "Message send successful";
    }

    void sendGameMessage(sf::TcpSocket& socket, const net::GameMessage& message) {
        net::NetMessage message1;
        *message1.mutable_game_message() = message;
        sendNetMessage(socket, message1);
    }
}

//namespace null {
//    void sendCommand(sf::TcpSocket& socket, const net::ClientCommand& commandMessage) {
//        LOGD << "Sending message";
//        sf::Packet packet;
//        packet << commandMessage.SerializeAsString();
//        auto status = socket.send(packet);
//        if (status != sf::Socket::Done) {
//            LOGD << "Cannot send message";
//            throw ReceiveException("Error occurred while trying to send message to client", status);
//        }
//        LOGD << "Message send successful";
//    }
//
//    net::SubscriberState receiveState(sf::TcpSocket& socket) {
//        net::SubscriberState message;
//        sf::Packet packet;
//        auto status = socket.receive(packet);
//        if (status != sf::Socket::Done) {
//            throw ReceiveException("Cannot receive message", status);
//        }
//        std::string resultString;
//        packet >> resultString;
//        message.ParseFromString(resultString);
//        return message;
//    }
//}
