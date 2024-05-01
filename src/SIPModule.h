#pragma once
#ifdef ARDUINO_ARCH_ESP32
#include "OpenKNX.h"
#include "ChannelOwnerModule.h"

class SIPModule : public SIPChannelOwnerModule
{
   void* _sipClient = nullptr;
   unsigned long _sipCallTimeoutMs = 0;
   unsigned long _sipCallSince = 0;
   uint8_t _currentChannel = 0;
  protected:
    OpenKNX::Channel* createChannel(uint8_t _channelIndex /* this parameter is used in macros, do not rename */) override; 

  public:
    SIPModule();

    void loop() override;

    const std::string name() override;
    const std::string version() override;
    void showInformations() override;
    void showHelp() override;
    bool connected();
    bool processCommand(const std::string cmd, bool diagnoseKo) override;

};

extern SIPModule openknxSIPModule;

#endif