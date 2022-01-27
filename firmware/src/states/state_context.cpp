#include "state_context.hpp"

#include "run_script_state.cpp"

namespace SyncBlink
{
    StateContext::StateContext(EventBus& eventBus, LED& led, Display& display, ScriptManager& scriptManager)
        : _eventBus(eventBus), _led(led), _display(display), _scriptManager(scriptManager)
    {
        _currentState = std::make_shared<RunScriptState>(*this);
    }

    void StateContext::loop()
    {
        _currentState->loop();
    }

    void StateContext::resetState()
    {
        _currentState = std::make_shared<RunScriptState>(*this);
    }

    void StateContext::changeState(std::shared_ptr<State> state)
    {
        _currentState = state;
    }

    LED& StateContext::getLed()
    {
        return _led;
    }
    Display& StateContext::getDisplay()
    {
        return _display;
    }
    EventBus& StateContext::getEventBus()
    {
        return _eventBus;
    }
    ScriptManager& StateContext::getScriptManager()
    {
        return _scriptManager;
    }
}