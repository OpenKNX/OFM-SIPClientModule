#pragma once
#include "OpenKNX.h"

class SIPCallNumberChannel : public OpenKNX::Channel
{
        volatile bool _triggered = false;
        std::string _name = std::string();
    public:
        SIPCallNumberChannel(uint8_t channelIndex);
        const std::string name() override;
        bool needCall();  
        const char* getPhoneNumber();
        uint8_t getCancelCallTime();
        void processInputKo(GroupObject &ko) override;
        bool processCommand(const std::string cmd, bool diagnoseKo);
};