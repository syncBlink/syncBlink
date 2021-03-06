#include "station_context.hpp"
#include "states/fail_safe_state.cpp"
#include "states/broadcast_script_state.cpp"
#include "states/receiving_firmware_state.cpp"
#include "views/splash_view.cpp"

namespace SyncBlink
{
    StationContext::StationContext() : _nodeManager(_led, _tcpServer, _wifi), _web(_wifi, _ScriptManager, _nodeManager)
    {
        resetState();
        checkException();
    }

    void StationContext::setup()
    {
        _display.init();
        _display.setView(std::make_shared<SyncBlink::SplashView>());
        _display.loop();
        _led.setup(LED_COUNT);

        _web.uploadListener
            .addEventHandler([this](float progress, bool isStart, bool isEnd, bool isError, uint64_t targetId) {
                if(isStart) currentState = std::make_shared<ReceivingFirmwareState>(*this);
                currentState->run(*this);
            });

        _wifi.connectWifi();
        _tcpServer.start();
        _udpDiscover.start(true);
    }

    void StationContext::loop()
    {
        #ifdef LOG_HEAP
        if(millis() - _lastHeapLog > 1000)
        {
            Serial.println(ESP.getFreeHeap());
            _lastHeapLog = millis();
        }
        #endif
        _display.setLeftStatus("");
        _display.setRightStatus(WiFi.localIP().toString().c_str());

        currentState->run(*this);
        
        _tcpServer.loop();
        _udpDiscover.loop();
        _led.loop();
        _web.loop();
        _display.loop();
    }
    
    void StationContext::resetState() 
    { 
        currentState = std::make_shared<BroadcastScriptState>(*this); 
    }

    void StationContext::checkException()
    {
        auto rstPtr = ESP.getResetInfoPtr();
        if(rstPtr->reason >= 1 && rstPtr->reason <= 4) currentState = std::make_shared<FailSafeState>(*this); 
    }

    LED& StationContext::getLed() { return _led; }
    Display& StationContext::getDisplay() { return _display; }
    ScriptManager& StationContext::getScriptManager() { return _ScriptManager; }
    SyncBlinkWeb& StationContext::getWebserver() { return _web; }
    TcpServer& StationContext::getTcpServer() { return _tcpServer; }
    NodeManager& StationContext::getNodeManager() { return _nodeManager; }
    uint64_t StationContext::getStationId() const { return _stationId; }
}