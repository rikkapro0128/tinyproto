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
#include "TinyLinkLayer.h"

#include <stdint.h>
#include <limits.h>

namespace tinyproto
{

class Proto
{
public:
    Proto(ILinkLayer &link, bool multithread = false);

    ~Proto();

    bool begin(ILinkLayer &link);

#if defined(ARDUINO)
    bool beginSerial();
#elif defined(__linux__) || defined(_WIN32)
    bool beginSerial(char *device);
#endif

    bool send(const IPacket &message, int timeout);

    bool read(IPacket &message, int timeout);

    bool readZeroCopy(IPacket &message, int timeout);

    void end();

private:
    uint8_t * m_buffer = nullptr;
    bool m_allocated = false;

    ILinkLayer *m_link = nullptr;
    bool m_multithread = false;
    tiny_events_t m_events{};

    int m_rxQueueSize = 1;
    IPacket *m_rxQueue = nullptr;

    void init();

    void onRead(uint8_t *buf, int len);

    void onSend(uint8_t *buf, int len);

    static void onReadCb(void *udata, uint8_t *buf, int len);

    static void onSendCb(void *udata, uint8_t *buf, int len);
};

}

