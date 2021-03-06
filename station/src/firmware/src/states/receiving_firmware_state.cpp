#ifndef RECEIVINGFIRMWARESTATE_H
#define RECEIVINGFIRMWARESTATE_H

#include "state.hpp"
#include "station_context.hpp"
#include "views/progress_view.cpp"
#include "send_firmware_state.cpp"

namespace SyncBlink
{
    class ReceivingFirmwareState : public State
    {
        public:
            ReceivingFirmwareState(StationContext& context) :
                _context(context),
                _progressView(std::make_shared<ProgressView>("Receiving Firmware..."))
            {
                _webEventHandleId = context.getWebserver()
                    .uploadListener
                    .addEventHandler([this](float progress, bool isStart, bool isEnd, bool isError, uint64_t targetId) 
                    {
                        _progressView->setProgress(progress);
                        _uploadDone = isEnd;
                        _targetId = targetId;
                    });
            }

            ~ReceivingFirmwareState()
            {
                _context.getWebserver()
                    .uploadListener
                    .removeEventHandler(_webEventHandleId);
            }

            void run(StationContext& context)
            {
                context.getLed().setAllLeds(SyncBlink::Blue);
                context.getDisplay().setView(_progressView);
                context.getDisplay().loop();

                if(_uploadDone) context.currentState = std::make_shared<SendFirmwareState>(context, _targetId);
            }

        private:
            StationContext& _context;
            std::shared_ptr<ProgressView> _progressView;            
            uint32_t _webEventHandleId = 0;
            uint64_t _targetId = 0;
            bool _uploadDone = false;
    };
}

#endif // RECEIVINGFIRMWARESTATE_H