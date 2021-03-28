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

#include "TinyPacket.h"
#include "TinyLightProtocol.h"
#include "TinyProtocolHdlc.h"
#include "TinyProtocolFd.h"
#include "TinySerial.h"

#include <stdint.h>
#include <limits.h>

namespace tinyproto
{

class ILinkLayer
{
public:
    virtual void begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) = 0;

    virtual void end() = 0;

    virtual void runRx() = 0;

    virtual void runTx() = 0;

    virtual int put(void *buf, int size) = 0;

    virtual int getMtu() { return INT_MAX; }
};

class IFdLinkLayer: public ILinkLayer
{
public:
    IFdLinkLayer(void *buffer, int size, int mtu = 64, int window = 2);

    void begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) override;

    void end() override;

    int put(void *buf, int size) override;

    int getMtu() override { return m_mtu; }

protected:
    tiny_fd_handle_t m_handle = nullptr;

private:
    uint8_t *m_buffer = nullptr;
    int m_bufferSize = 0;
    int m_mtu = 64;
    uint8_t m_window = 2;
    int m_sendTimeout = 100;
    hdlc_crc_t m_crc = HDLC_CRC_8;
};

template <int Z, int B>
class ISerialLinkLayer: public IFdLinkLayer
{
public:
    ISerialLinkLayer(char *dev, int timeout): IFdLinkLayer( m_buffer, Z ), m_timeout( timeout ), m_serial( dev )
    {
    }

    void begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) override
    {
        IFdLinkLayer::begin(onReadCb, onSendCb, udata);
        m_serial.setTimeout( m_timeout );
        m_serial.begin( m_speed );
    }

    void end() override
    {
        m_serial.end();
        IFdLinkLayer::end();
    }

    void runRx() override
    {
        uint8_t buf[B];
        int len = m_serial.readBytes(buf, B);
        tiny_fd_on_rx_data( m_handle, buf, len );
    }

    void runTx() override
    {
        uint8_t buf[B];
        int len = tiny_fd_get_tx_data( m_handle, buf, B );
        uint8_t *ptr = buf;
        while (len > 0)
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
    uint8_t m_buffer[Z]{};
    int m_speed = 115200;
    int m_timeout;
    tinyproto::Serial m_serial;
};

#if defined(__linux__) || defined(_WIN32)

class SerialLinkLayer: public ISerialLinkLayer<1000, 128>
{
public:
    SerialLinkLayer( char *dev, int timeout ): ISerialLinkLayer( dev, timeout )
    {
    }

};

#endif

}

