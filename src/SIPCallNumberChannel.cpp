#include "SIPCallNumberChannel.h"

SIPCallNumberChannel::SIPCallNumberChannel(uint8_t channelIndex)
{
    _channelIndex = channelIndex;
     char buffer[10] = {0};
    _name = itoa(channelIndex, buffer, 10);
}

const std::string SIPCallNumberChannel::name()
{
    return _name;
}

bool SIPCallNumberChannel::needCall()
{
    auto result = _triggered;
    if (result)
        _triggered = false;
    return result;
}

const char *SIPCallNumberChannel::getPhoneNumber()
{
    return (const char *)ParamSIP_CHPhoneNumber;
}

uint8_t SIPCallNumberChannel::getCancelCallTime()
{
    return ParamSIP_CHCancelCall;
}

void SIPCallNumberChannel::processInputKo(GroupObject &ko)
{
      // channel ko
    auto index = SIP_KoCalcIndex(ko.asap());
    switch (index)
    {
        case SIP_KoCHPhoneNumber:
            if (ko.value(DPT_Trigger))
                _triggered = true;
            break;
    }
}

bool SIPCallNumberChannel::processCommand(const std::string cmd, bool diagnoseKo)
{
    if (cmd == "call")
    {
        KoSIP_CHPhoneNumber.value(true, DPT_Trigger);
        return true;
    }
    return false;
}