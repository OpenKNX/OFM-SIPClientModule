#ifdef ARDUINO_ARCH_ESP32

#include "SIPModule.h"
#include "SIPCallNumberChannel.h"
#include "WiFi.h"
#include "lwip/ip_addr.h"

#include "sip_client/lwip_udp_client.h"
#include "sip_client/mbedtls_md5.h"
#include "sip_client/sip_client.h"

using SipClientT = SipClient<LwipUdpClient, MbedtlsMd5>;


SIPModule::SIPModule()
 : SIPChannelOwnerModule(SIP_ChannelCount)
{

}

const std::string SIPModule::name()
{
    return "SIP";
}

const std::string SIPModule::version()
{
#ifdef MODULE_SIPClientModule_Version
    return MODULE_SIPClientModule_Version;
#else
    // hides the module in the version output on the console, because the firmware version is sufficient.
    return "";
#endif
}

void SIPModule::showInformations()
{
    // if (connected())
    //     openknx.logger.logWithPrefixAndValues("SIP", "Connected (%s)", WiFi.localIP().toString().c_str());
    // else
    //     openknx.logger.logWithPrefix("SIP", "Not connected");
}

void SIPModule::showHelp()
{
    openknx.console.printHelpLine("sip<CC> call", "Call the number which is configured in channel CC. i.e. sip1 call");
}


bool SIPModule::processCommand(const std::string cmd, bool diagnoseKo)
{
    if (cmd.rfind("sip", 0) == 0)
    {
        auto channelString = cmd.substr(3);
        if (channelString.length() > 0)
        {
            auto pos = channelString.find_first_of(' ');
            std::string channelNumberString;
            std::string channelCmd;
            if (pos > 0 && pos != std::string::npos)
            {
                channelNumberString = channelString.substr(0, pos);
                channelCmd = channelString.substr(pos + 1);
            }
            else
            {
                channelNumberString = channelString;
                channelCmd = "";
            }
            auto channel = atoi(channelNumberString.c_str());
            if (channel < 1 || channel > getNumberOfChannels())
            {
                logInfoP("Channel %d not found", channel);
                return true;
            }
            auto sipCallNumberChannel = (SIPCallNumberChannel*) getChannel(channel - 1);
            if (sipCallNumberChannel == nullptr)
            {
                logInfoP("Channel not %d activated", channel);
                return true;
            }
            if (channelCmd.length() != 0)
            {
                return sipCallNumberChannel->processCommand(channelCmd, diagnoseKo);
            }
        }
    }
    return false;
}


OpenKNX::Channel* SIPModule::createChannel(uint8_t _channelIndex /* this parameter is used in macros, do not rename */)
{
    return new SIPCallNumberChannel(_channelIndex);
}

void SIPModule::loop()
{
#if ARDUINO_ARCH_ESP32 
    auto sipClient = (SipClientT*)_sipClient;
    if (sipClient != nullptr)
    {
        sipClient->run();
        if (_sipCallSince > 0)
        {
            // SIP call active
            auto now = millis();
            if (now - _sipCallSince > _sipCallTimeoutMs)
            {
                _sipCallSince = 0;
                logDebugP("Cancel call");
                sipClient->request_cancel();
            }
        }
        else
        {
            
            auto channel = (SIPCallNumberChannel*) getChannel(_currentChannel);
            _currentChannel++;
            if (_currentChannel >= getNumberOfChannels())
                _currentChannel = 0;
            if (channel != nullptr && channel->needCall())
            {
                _sipCallSince = millis();
                if (_sipCallSince == 0)
                    _sipCallSince++;
                _sipCallTimeoutMs = channel->getCancelCallTime() * 1000;
                std::string phoneNumber = channel->getPhoneNumber();
                logDebugP("Call phone number %s", phoneNumber.c_str());
                sipClient->request_ring(phoneNumber, "555");
            }
        }
    }
    else if (WiFi.status() == WL_CONNECTED)
    {
        auto user = std::string((const char*)ParamSIP_SIPUser);
        auto password = std::string((const char*)ParamSIP_SIPPassword);

        std::string serverIP;
        if (ParamSIP_UseIPGateway)
        {
            serverIP = std::string(WiFi.gatewayIP().toString().c_str());
        }
        else
        {
            auto parameterIP = (uint32_t)ParamSIP_SIPGatewayIP;
            uint32_t arduinoIP = ((parameterIP & 0xFF000000) >> 24) | ((parameterIP & 0x00FF0000) >> 8) | ((parameterIP & 0x0000FF00) << 8) | ((parameterIP & 0x000000FF) << 24);
            serverIP = IPAddress(arduinoIP).toString().c_str();
        }
        uint16_t serverPort = ParamSIP_SIPGatewayPort;
        char buffer[10] = {0};
        std::string serverPortStr = itoa(serverPort, buffer, 10);
        auto localIP = std::string(WiFi.localIP().toString().c_str());
 
        logDebugP("User: %s", user.c_str());
        logDebugP("Server IP: %s", serverIP.c_str());
        logDebugP("Port: %s", serverPortStr.c_str());
        logDebugP("Local IP: %s", localIP.c_str());
        auto sipClient = new SipClientT(user, password, serverIP, serverPortStr, localIP);
        _sipClient = sipClient;
        bool initialized = sipClient->init();
        logDebugP("SIP Client inialized: %d", (int) initialized);
    }
    SIPChannelOwnerModule::loop();
#endif
}

SIPModule openknxSIPModule;
#endif