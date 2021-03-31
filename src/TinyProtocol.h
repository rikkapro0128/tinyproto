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
#include "TinySerialLinkLayer.h"

#include <stdint.h>
#include <limits.h>

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

private:
    ILinkLayer *m_link = nullptr;
    bool m_multithread = false;
    uint8_t *m_message = nullptr;
    int m_messageLen;

    tiny_events_t m_events{};

    void onRead(uint8_t *buf, int len);

    void onSend(uint8_t *buf, int len);

    static void onReadCb(void *udata, uint8_t *buf, int len);

    static void onSendCb(void *udata, uint8_t *buf, int len);
};



class SerialProto: public Proto
{
public:
    SerialProto(char *dev, bool multithread = false);

    SerialLinkLayer &getLink();

private:
    SerialLinkLayer m_layer;
};


} // namespace tinyproto
