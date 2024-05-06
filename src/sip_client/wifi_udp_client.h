/*
   Copyright 2017 Christian Taedcke <hacking@taedcke.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#pragma once
#include "OpenKNX.h"
#include <array>
#include <string>
#include <cstring>

#include "WiFiUdp.h"

static constexpr const int RX_BUFFER_SIZE = 2048;
static constexpr const int TX_BUFFER_SIZE = 2048;


template<std::size_t SIZE>
class Buffer
{
public:
    Buffer()
    {
        clear();
    }

    void clear()
    {
        m_buffer[0] = '\0';
    }

    Buffer<SIZE>& operator<<(const char* str)
    {
        strncat(m_buffer.data(), str, m_buffer.size() - strlen(m_buffer.data()) - 1);
        return *this;
    }
    Buffer<SIZE>& operator<<(const std::string& str)
    {
        strncat(m_buffer.data(), str.c_str(), m_buffer.size() - strlen(m_buffer.data()) - 1);
        return *this;
    }
    Buffer<SIZE>& operator<<(int8_t i)
    {
        snprintf(m_buffer.data() + strlen(m_buffer.data()), m_buffer.size() - strlen(m_buffer.data()), "%c", i);
        return *this;
    }
    Buffer<SIZE>& operator<<(uint8_t i)
    {
        snprintf(m_buffer.data() + strlen(m_buffer.data()), m_buffer.size() - strlen(m_buffer.data()), "%c", i);
        return *this;
    }
    Buffer<SIZE>& operator<<(uint16_t i)
    {
        snprintf(m_buffer.data() + strlen(m_buffer.data()), m_buffer.size() - strlen(m_buffer.data()), "%d", i);
        return *this;
    }
    Buffer<SIZE>& operator<<(uint32_t i)
    {
        snprintf(m_buffer.data() + strlen(m_buffer.data()), m_buffer.size() - strlen(m_buffer.data()), "%d", i);
        return *this;
    }

    const char* data() const
    {
        return m_buffer.data();
    }

    size_t size() const
    {
        return strlen(m_buffer.data());
    }

private:
    std::array<char, SIZE> m_buffer;
};

using TxBufferT = Buffer<TX_BUFFER_SIZE>;

class WifiUdpClient
{
public:
    WifiUdpClient(const std::string& server, const std::string& server_port, uint16_t local_port)
    : m_server_port(atoi(server_port.c_str()))
    , m_server(server)
    , m_local_port(local_port)
    , m_wifiUdp()
    , m_initialized(false)
    , m_logPrefix("SIP UDP")
    {
        m_useIp = m_server_ip.fromString(m_server.c_str());
    }

    ~WifiUdpClient()
    {
    }

    const std::string logPrefix()
    {
        return m_logPrefix;
    }


    void set_server_ip(const std::string& server)
    {
        if (is_initialized())
        {
            deinit();
        }
        m_server = server;
        m_useIp = m_server_ip.fromString(m_server.c_str());
    }
    void set_server_port(const std::string& server_port)
    {
        if (is_initialized())
        {
            deinit();
        }
        m_server_port = atoi(server_port.c_str());
    }

    void deinit()
    {
        if (!is_initialized())
        {
            return;
        }
        m_wifiUdp.stop();
        m_initialized = false;
    }

    bool init()
    {
        if (m_initialized)
        {
            logErrorP("Socket already initialized");
            return false;
        }
    
        auto result = m_wifiUdp.begin(m_local_port);
        m_initialized = result != 0;
        return m_initialized;
    }

    bool is_initialized() const
    {
        return m_initialized;
    }


    std::string receive(uint32_t timeout_msec)
    {
        m_wifiUdp.setTimeout(timeout_msec);
        auto size = m_wifiUdp.parsePacket();
        if (size)
        {

            auto result = m_wifiUdp.readString();
            logTraceP("Received %d bytes",size);
            return std::string(result.c_str());
        }
        return std::string();
    }

    TxBufferT& get_new_tx_buf()
    {
        m_tx_buffer.clear();
        return m_tx_buffer;
    }

    bool send_buffered_data()
    { 
        logDebugP("Sending %d bytes", m_tx_buffer.size());
        // logDebugP("Sending following data: %s", m_tx_buffer.data());
        if (m_useIp)
            m_wifiUdp.beginPacket(m_server_ip, m_server_port);
        else
            m_wifiUdp.beginPacket(m_server.c_str(), m_server_port);

        auto result = m_wifiUdp.write((const uint8_t*) m_tx_buffer.data(), m_tx_buffer.size());
        bool endPacketResult = m_wifiUdp.endPacket();
        if (result != m_tx_buffer.size() || endPacketResult == 0)
        {         
            logErrorP("Failed to send data %d, errno=%d", result, errno);
        }
        return result == m_tx_buffer.size() && endPacketResult != 0;
    }
private:
    uint16_t m_server_port;
    IPAddress m_server_ip;
    bool m_useIp;
    std::string m_server;
    const uint16_t m_local_port;
    bool m_initialized;
    std::string m_logPrefix;

    TxBufferT m_tx_buffer;
    std::array<char, RX_BUFFER_SIZE> m_rx_buffer;
    WiFiUDP m_wifiUdp;
};
