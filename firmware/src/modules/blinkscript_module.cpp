#include "blinkscript_module.hpp"
#include "core/audio/analyzer_constants.hpp"
#include "core/event/commands/set_display.hpp"

namespace SyncBlink
{
    BlinkScriptModule::BlinkScriptModule(LED& led, EventBus& eventBus) : BlinkScriptModule(led, eventBus, Script())
    { }

    BlinkScriptModule::BlinkScriptModule(LED& led, EventBus& eventBus, Script initalScript) : _led(led), _eventBus(eventBus), _currentScript(initalScript)
    {   
        _scriptEventHandleId = _eventBus.addEventHandler<Events::ScriptChangeEvent>(this);
        _analyzerEventHandleId = _eventBus.addEventHandler<Events::AnalyzerUpdateEvent>(this);
        
        _runScriptView = std::make_shared<RunScriptView>();
        _invalidScriptView = std::make_shared<IconTextView>("Invalid script!", u8g2_font_open_iconic_check_2x_t, 66);
        _failSafeView = std::make_shared<IconTextView>("Fail Safe!", u8g2_font_open_iconic_thing_2x_t, 78);
    }
    
    BlinkScriptModule::~BlinkScriptModule()
    {
        _eventBus.removeEventHandler(_scriptEventHandleId);
        _eventBus.removeEventHandler(_analyzerEventHandleId);
    }

    void BlinkScriptModule::loop()
    {   
        if ((_activeScriptChanged || _blinkScript == nullptr) && _currentScript.Exists)
        {
            Commands::SetDisplay command = { _runScriptView, _currentScript.Name };
            _eventBus.trigger(command);

            _blinkScript = std::make_shared<BlinkScript>(_led, _currentScript.Content, MaxFrequency);
            _blinkScript->updateLedInfo(0, 0, _led.getLedCount());
            _blinkScript->init();
        }
    }

    void BlinkScriptModule::onEvent(const Events::AnalyzerUpdateEvent& event)
    {
        if (_blinkScript == nullptr) return;

        uint32_t delta = millis() - _lastLedUpdate;
        _lastLedUpdate = millis();

        setView(event, delta);
        _blinkScript->updateAnalyzerResult(event.volume, event.frequency);
        _blinkScript->run(delta);
    }

    void BlinkScriptModule::onEvent(const Events::ScriptChangeEvent& event)
    {
        _activeScriptChanged = true;
        _currentScript = event.script;
    }

    void BlinkScriptModule::onEvent(const Events::MeshUpdateEvent& event)
    {
        _activeScriptChanged = true;
        _currentScript = event.script;

        Serial.println("test");
        Serial.println(event.script.Name.c_str());
        Serial.println(event.script.Content.c_str());
    }

    bool BlinkScriptModule::checkBlinkScript()
    {
        bool valid = true;
        if (_blinkScript->isFaulted())
        {
            Commands::SetDisplay command = { _invalidScriptView, _currentScript.Name };
            _eventBus.trigger(command);
            valid = false;
        }
        return valid;
    }

    void BlinkScriptModule::setView(Events::AnalyzerUpdateEvent event, uint32_t delta)
    {
        _runScriptView->delta = delta;
        _runScriptView->volume = event.volume;
        _runScriptView->decibel = event.decibel;
        if (event.volume > 0 && event.frequency > 0)
        {
            _runScriptView->dominantFrequency = event.frequency;
            _runScriptView->setFreqBars(event.freqBins);
        }
        else
        {
            _runScriptView->dominantFrequency = 0;
            _runScriptView->fadeFrequencyRange();
        }
    }
}