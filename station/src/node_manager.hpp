#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <mesh/syncblink_mesh.hpp>
#include <tcp/tcp_server.hpp>
#include <messages/client_messages.hpp>
#include <messages/server_messages.hpp>
#include <vector>

namespace SyncBlink
{
    const std::string FirmwarePath = "/firmware.bin";

    class NodeManager
    {
        public:
            void addNode(Client::ConnectionMessage connectionMessage);
            void removeNode(uint64_t nodeId);

            void setLightMode(bool lightMode);
            void setAnalyzer(uint64_t analyzerId);
            void pingNode(uint64_t nodeId);
            void renameNode(uint64_t nodeId, const std::string& label);

            bool getLightMode() const;
            uint64_t getActiveAnalyzer() const;
            uint32_t getTotalLedCount() const;
            uint32_t getTotalNodeCount() const;
            std::vector<Client::ConnectionMessage> getConnectedNodes() const;

        private:
            void countInfos();
            
            uint32_t _totalLeds;
            uint32_t _totalNodes;
            bool _lightMode = false;
            uint64_t _activeAnalyzer = SyncBlink::getId();

            std::vector<Client::ConnectionMessage> _connectedNodes = {{ true, true, false, SyncBlink::getId(), 0, LED_COUNT, VERSIONMAJOR, VERSIONMINOR, { 'S', 't', 'a', 't', 'i', 'o', 'n' } }};
    };
}

#endif // NODEMANAGER_H