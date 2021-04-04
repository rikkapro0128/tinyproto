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
#include "TinyFdLinkLayer.h"

#if defined(ARDUINO)
#include "proto/fd/tiny_fd_int.h"
#endif

namespace tinyproto
{

template <int MTU, int TX_WINDOW, int BUFFER_SIZE, int BLOCK> class StaticSerialFdLinkLayer: public ISerialLinkLayer<IFdLinkLayer, BLOCK>
{
public:
    StaticSerialFdLinkLayer(char *dev)
        : ISerialLinkLayer<IFdLinkLayer, BLOCK>(dev, this->m_buffer, BUFFER_SIZE)
    {
        this->setMtu(MTU);
        this->setWindow(TX_WINDOW);
    }

private:
    uint8_t m_buffer[BUFFER_SIZE];
};


#if defined(ARDUINO)

/** Valid only for Arduino IDE, since it has access to internal headers */
template <int MTU, int TX_WINDOW, int RX_WINDOW, int BLOCK> using ArduinoStaticSerialFdLinkLayer = StaticSerialFdLinkLayer<MTU, TX_WINDOW, FD_BUF_SIZE_EX(MTU, TX_WINDOW, HDLC_CRC_16, RX_WINDOW), BLOCK>;

class ArduinoSerialFdLink: public ArduinoStaticSerialFdLinkLayer<32, 2, 2, 4>
{
public:
    ArduinoSerialFdLink(HardwareSerial *dev)
        : ArduinoStaticSerialFdLinkLayer<32, 2, 2, 4>(reinterpret_cast<char *>(dev))
    {
    }
};

#endif

class SerialFdLink: public ISerialLinkLayer<IFdLinkLayer, 128>
{
public:
    SerialFdLink(char *dev)
        : ISerialLinkLayer<IFdLinkLayer, 128>(dev, nullptr, 0)
    {
    }

    ~SerialFdLink();

    bool begin(on_frame_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata) override;

    void end() override;

private:
    uint8_t *m_buffer = nullptr;
};

} // namespace tinyproto
