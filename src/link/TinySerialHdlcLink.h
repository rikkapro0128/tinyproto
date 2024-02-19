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

#pragma once

#include "TinySerialLinkLayer.h"
#include "TinyHdlcLinkLayer.h"

#if defined(ARDUINO)
#include "proto/hdlc/low_level/hdlc_int.h"
#endif

namespace tinyproto
{

template <int MTU, int BUFFER_SIZE, int BLOCK> class StaticSerialHdlcLink: public ISerialLinkLayer<IHdlcLinkLayer, BLOCK>
{
public:
    StaticSerialHdlcLink(char *dev)
        : ISerialLinkLayer<IHdlcLinkLayer, BLOCK>(dev, this->m_buffer, BUFFER_SIZE)
    {
        this->setMtu(MTU);
    }

private:
    uint8_t m_buffer[BUFFER_SIZE];
};


#if defined(ARDUINO)

/** Valid only for Arduino IDE, since it has access to internal headers */
template <int MTU, int RX_WINDOW, int BLOCK> using ArduinoStaticSerialHdlcLinkLayer = StaticSerialHdlcLink<MTU, HDLC_BUF_SIZE_EX(MTU, HDLC_CRC_16, RX_WINDOW), BLOCK>;

class ArduinoSerialHdlcLink: public ArduinoStaticSerialHdlcLinkLayer<32, 2, 4>
{
public:
    ArduinoSerialHdlcLink(HardwareSerial *dev)
        : ArduinoStaticSerialHdlcLinkLayer<32, 2, 4>(reinterpret_cast<char *>(dev))
    {
    }
};

#endif

class SerialHdlcLink: public ISerialLinkLayer<IHdlcLinkLayer, 32>
{
public:
    SerialHdlcLink(char *dev)
        : ISerialLinkLayer<IHdlcLinkLayer, 32>(dev, nullptr, 0)
    {
    }

    ~SerialHdlcLink();

    bool begin(on_frame_read_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata) override;

    void end() override;

private:
    uint8_t *m_buffer = nullptr;
};

} // namespace tinyproto
