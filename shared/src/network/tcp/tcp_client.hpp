#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "tcp_stream.hpp"
#include "event_registration.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <functional>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    typedef std::function<void(std::string mod)> MeshModEvent;
    typedef std::function<void(bool isConnected)> ClientConnectionEvent;

    typedef std::function<void(AudioAnalyzerMessage message)> AudioAnalyzerEvent;
    typedef std::function<void(Server::UpdateMessage message)> MeshUpdateEvent;
    typedef std::function<void(Server::SourceMessage message)> SourceUpdateEvent;
    typedef std::function<void(Server::NodeRenameMessage message)> NodeRenameEvent;
    typedef std::function<void(std::vector<uint8_t> data, uint64_t targetClientId, Server::MessageType messageType)> FirmwareFlashEvent;

    class TcpClient
    {
    public:
        void start(String _serverIp);
        void loop();
        
        void sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType);

        bool isConnected();

        EventRegistration<MeshModEvent> meshModEvents;
        EventRegistration<ClientConnectionEvent> connectionEvents;
        EventRegistration<AudioAnalyzerEvent> audioAnalyzerEvents;
        EventRegistration<MeshUpdateEvent> meshUpdateEvents;
        EventRegistration<SourceUpdateEvent> sourceUpdateEvents;
        EventRegistration<NodeRenameEvent> nodeRenameEvents;
        EventRegistration<FirmwareFlashEvent> firmwareFlashEvents;

    private:
        void checkConnection();
        void handleIncomingMessages();

        TcpStream _client;
        String _serverIp;
        bool _wasConnected = false;
    };
}

#endif // TCPCLIENT_H