/*
    Copyright 2016-2021 (C) Alexey Dynda

    This file is part of Tiny Protocol Library.

    Protocol Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Protocol Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 This is Tiny protocol implementation for microcontrollers

 @file
 @brief Tiny protocol Arduino API

*/

#pragma once

#include "TinyPacket.h"
#include "TinyLightProtocol.h"
#include "TinyProtocolHdlc.h"
#include "TinyProtocolFd.h"
#include "link/TinyLinkLayer.h"
#include "link/TinySerialFdLink.h"

#include "hal/tiny_types.h"

#include <stdint.h>
#include <limits.h>

#if CONFIG_TINYHAL_THREAD_SUPPORT == 1
#include <thread>
#endif

namespace tinyproto
{

class Proto
{
public:
    Proto(bool multithread = false);

    ~Proto();

    void setLink(ILinkLayer &link);

    ILinkLayer &getLink();

    bool begin();

    bool send(const IPacket &message, uint32_t timeout);

    bool read(IPacket &message, uint32_t timeout);

    void end();

#if CONFIG_TINYHAL_THREAD_SUPPORT == 1
    void setTxDelay( uint32_t delay );
#endif

private:
    ILinkLayer *m_link = nullptr;
    bool m_multithread = false;
    uint8_t *m_message = nullptr;
    int m_messageLen;
    bool m_terminate = true;
#if CONFIG_TINYHAL_THREAD_SUPPORT == 1
    std::thread *m_sendThread = nullptr;
    std::thread *m_readThread = nullptr;
    uint32_t m_txDelay = 0;
#endif

    tiny_events_t m_events{};

    void onRead(uint8_t *buf, int len);

    void onSend(const uint8_t *buf, int len);

    static void onReadCb(void *udata, uint8_t *buf, int len);

    static void onSendCb(void *udata, const uint8_t *buf, int len);

#if CONFIG_TINYHAL_THREAD_SUPPORT == 1
    void runTx();

    void runRx();
#endif

};



class SerialFdProto: public Proto
{
public:
    SerialFdProto(char *dev, bool multithread = false);

    SerialFdLink &getLink();

private:
    SerialFdLink m_layer;
};


} // namespace tinyproto
