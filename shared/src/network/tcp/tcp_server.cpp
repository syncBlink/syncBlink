#include "tcp_server.hpp"
#include "network/mesh/syncblink_mesh.hpp"

namespace SyncBlink
{
    void TcpServer::start()
    {
        _server.begin();
    }

    void TcpServer::loop()
    {
        clearClients();
        checkNewClients();
        handleIncomingMessages();
    }

    void TcpServer::broadcast(void* body, uint32_t bodySize, Server::MessageType messageType)
    {
        auto packet = Message::toMessagePacket(body, bodySize, messageType);
        #ifdef DEBUG_TCP
        if(_clients.size() > 0) Serial.printf("[TCP] Writing message - Type: %i, Size: %i\n", messageType, packet.size());
        #endif
        for(TcpClient& client : _clients)
        {
            client.writeMessage(packet);
        }
    }

    uint32_t TcpServer::getClientsCount()
    {
        return _clients.size();
    }

    void TcpServer::clearClients()
    {
        for(auto iter = _clients.begin(); iter != _clients.end();)
        {
            // check if client is still connected to ap
            bool connectedToAp = false;
            struct station_info* statInfo = wifi_softap_get_station_info();
            while (statInfo != nullptr)
            {
                IPAddress clientIp = IPAddress((&statInfo->ip)->addr);
                if(clientIp == iter->getRemoteIp())
                {
                    connectedToAp = true;
                    break;
                }
                statInfo = STAILQ_NEXT(statInfo, next);
            }
            wifi_softap_free_station_info();

            // Analyzers are not connected to the AP. Thats why we check the client AND the AP connection
            if(iter->isWriteTimeout() || (!connectedToAp && !iter->isConnected()))
            {                
                if(iter->getStreamId() != 0)
                {
                    #ifdef DEBUG_TCP
                    Serial.printf("[TCP SERVER] Client lost connection: %12llx (AP Connected: %i, Write Timeout: %i)\n", iter->getStreamId(), connectedToAp, iter->isWriteTimeout());
                    #endif
                    for (auto event : serverDisconnectionEvents.getEventHandlers())
                        event.second(iter->getStreamId());
                }
                iter->flush();
                iter->stop();

                // We explicity abandon without reset (this will also purge the unsent message memory)
                for(auto pcb = tcp_active_pcbs; pcb != nullptr; pcb = pcb->next)
                {
                    IPAddress ip = pcb->remote_ip;
                    if(ip == iter->getRemoteIp())
                    {
                        tcp_abandon(pcb, 0);
                        break;
                    }
                }

                _clients.erase(iter);
            }
            else iter++;
        }
    }

    void TcpServer::checkNewClients()
    {
        if(_server.hasClient())
        {
            _clients.push_back(TcpClient(_server.available()));
        }
    }

    void TcpServer::handleIncomingMessages()
    {
        for(auto& client : _clients)
        {
            Message message;
            if(Message::available(client.getWiFiClient(), message))
            {
                for (auto event : serverMessageEvents.getEventHandlers())
                    event.second(message);

                switch(message.type)
                {
                    case Client::MESH_CONNECTION:
                    {
                        auto conMessage = message.as<Client::ConnectionMessage>();
                        if(conMessage.parentId == 0)
                        {
                            if(conMessage.isNode) conMessage.parentId = SyncBlink::getId();
                            memcpy(&message.body[0], &conMessage, sizeof(conMessage));
                            client.setStreamId(conMessage.nodeId);

                            #ifdef DEBUG_TCP
                            if(conMessage.isAnalyzer && !conMessage.isNode)
                            {
                                Serial.printf("[TCP SERVER] New Analyzer connected: %s\n", conMessage.nodeLabel);
                            }
                            else
                            {
                                Serial.printf("[TCP SERVER] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                                    conMessage.nodeId, conMessage.ledCount,
                                    conMessage.parentId, conMessage.majorVersion, conMessage.minorVersion);
                            }                    
                            #endif

                            for (auto event : serverConnectionEvents.getEventHandlers())
                                event.second(conMessage);
                        }
                        break;
                    }
                    case Client::MESH_DISCONNECTION:
                    {
                        auto nodeId = message.as<uint64_t>();
                        for (auto event : serverDisconnectionEvents.getEventHandlers())
                            event.second(nodeId);
                        break;
                    }
                    case Client::SCRIPT_DISTRIBUTED:
                    {
                        for (auto event : serverScriptDistributedEvents.getEventHandlers())
                            event.second();
                        break;
                    }
                    case Client::EXTERNAL_ANALYZER:
                    {
                        auto analyzerMessage = message.as<AudioAnalyzerMessage>();
                        for (auto event : serverExternalAnalyzerEvent.getEventHandlers())
                            event.second(analyzerMessage);
                        break;
                    }
                }
            }
        }
    }
}