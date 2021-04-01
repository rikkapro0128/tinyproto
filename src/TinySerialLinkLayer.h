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

#include "TinyLinkLayer.h"
#include "TinySerial.h"

#include "proto/fd/tiny_fd_int.h"

namespace tinyproto
{

template <class BASE, int BSIZE> class ISerialLinkLayer: public BASE
{
public:
    ISerialLinkLayer(char *dev, void *buffer, int size)
        : BASE(buffer, size)
        , m_serial(dev)
    {
    }

    bool begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) override
    {
        bool result = BASE::begin(onReadCb, onSendCb, udata);
        m_serial.setTimeout( this->getTimeout() );
        return result && m_serial.begin(m_speed);
    }

    void end() override
    {
        m_serial.end();
        BASE::end();
    }

    void runRx() override
    {
        uint8_t buf[BSIZE];
        int len = m_serial.readBytes(buf, BSIZE);
        BASE::parseData( buf, len);
    }

    void runTx() override
    {
        uint8_t buf[BSIZE];
        int len = BASE::getData( buf, BSIZE);
        uint8_t *ptr = buf;
        while ( len > 0 )
        {
            int sent = m_serial.write(ptr, len);
            if ( sent < 0 )
            {
                break;
            }
            ptr += sent;
            len -= sent;
        }
    }

private:
    int m_speed = 115200;
    tinyproto::Serial m_serial;
};

template <int MTU, int TX_WINDOW, int RX_WINDOW, int BLOCK> class StaticSerialFdLinkLayer: public ISerialLinkLayer<IFdLinkLayer, BLOCK>
{
public:
    StaticSerialFdLinkLayer(char *dev)
        : ISerialLinkLayer<IFdLinkLayer, BLOCK>(dev, this->m_buffer, FD_BUF_SIZE_EX(MTU, TX_WINDOW, HDLC_CRC_16, RX_WINDOW))
    {
        this->setMtu(MTU);
        this->setWindow(TX_WINDOW);
    }

private:
    uint8_t m_buffer[FD_BUF_SIZE_EX(MTU, TX_WINDOW, HDLC_CRC_16, RX_WINDOW)];
};

#if defined(ARDUINO)

class SerialFdLink: public StaticSerialFdLinkLayer<16, 2, 2, 4>
{
public:
    SerialFdLink(HardwareSerial *dev)
        : StaticSerialFdLinkLayer(retinterpret_cast<char *>(dev))
    {
    }
};

#else

class SerialFdLink: public ISerialLinkLayer<IFdLinkLayer, 128>
{
public:
    SerialFdLink(char *dev)
        : ISerialLinkLayer<IFdLinkLayer, 128>(dev, nullptr, 0)
    {
    }

    ~SerialFdLink();

    bool begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) override;

    void end() override;

private:
    uint8_t *m_buffer = nullptr;
};

#endif

} // namespace tinyproto
