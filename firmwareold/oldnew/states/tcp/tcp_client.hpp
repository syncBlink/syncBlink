#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "event_registration.hpp"
#include "messages/message.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <functional>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    class TcpClient
    {
    public:
        TcpClient();
        TcpClient(WiFiClient client);

        void start(String serverIp, uint16_t port);
        void stop();
        void flush();

        void loop();
                
        void sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType);
        void writeMessage(std::vector<uint8_t> message);

        bool isConnected();
        bool isDiscontinued();
        bool isWriteTimeout();

        void setStreamId(uint64_t id);
        uint64_t getStreamId() const;

        WiFiClient& getWiFiClient();
        IPAddress getRemoteIp();

    private:
        void checkConnection();
        bool connectTo(String socketIp, uint16_t port);
        void handleIncomingMessages();

        WiFiClient _client;
        String _serverIp;
        uint16_t _port;

        uint8_t _retryCount = 0;
        bool _wasConnected = false;
        uint64_t _streamId = 0;
        bool _writeTimeout = false;
    };
}

#endif // TCPCLIENT_H