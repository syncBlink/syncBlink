#ifndef FAILSAFESTATE_H
#define FAILSAFESTATE_H

#include <led.hpp>
#include <display.hpp>

#include "state.hpp"
#include "event/event_bus.hpp"
#include "views/icon_text_view.cpp"
#include "scripting/script_manager.hpp"
#include "event/events/script_change_event.hpp"

namespace SyncBlink
{
    class FailSafeState : public State, public EventHandler<Events::ScriptChangeEvent>
    {
        public:
            FailSafeState(EventBus& eventBus, LED& led, Display& display) :
                _eventBus(eventBus),
                _led(led),
                _display(display)
            {
                _scriptEventHandleId = _eventBus.addEventHandler<Events::ScriptChangeEvent>(this);
                _failSafeView = std::make_shared<IconTextView>("Fail Safe!", u8g2_font_open_iconic_thing_2x_t, 78);
            }

            ~FailSafeState()
            {
                _eventBus.removeEventHandler(_scriptEventHandleId);
            }

            void loop()
            {
                _led.setAllLeds(Colors::Yellow);
                _display.setView(_failSafeView);
                
                //(_activeScriptChanged) context.resetState();
            }

            void onEvent(const Events::ScriptChangeEvent& event)
            {
                _activeScriptChanged = true;
            }

        private:
            EventBus& _eventBus;
            LED& _led;
            Display& _display;
            std::shared_ptr<IconTextView> _failSafeView;            
            uint32_t _scriptEventHandleId = 0;
            bool _activeScriptChanged = false;
    };
}

#endif // FAILSAFESTATE_H