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
#include "TinyProtocolFd.h"

#include <stdint.h>
#include <limits.h>

namespace tinyproto
{

class ILinkLayer
{
public:
    virtual bool begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) = 0;

    virtual void end() = 0;

    virtual void runRx() = 0;

    virtual void runTx() = 0;

    virtual int put(void *buf, int size) = 0;

    virtual int getMtu()
    {
        return INT_MAX;
    }
};

class IFdLinkLayer: public ILinkLayer
{
public:
    IFdLinkLayer(void *buffer, int size);

    ~IFdLinkLayer();

    bool begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) override;

    void end() override;

    int put(void *buf, int size) override;

    int getMtu() override
    {
        return m_mtu;
    }

    int getWindow()
    {
        return m_txWindow;
    }

    hdlc_crc_t getCrc()
    {
        return m_crc;
    }

    void setMtu(int mtu)
    {
        m_mtu = mtu;
    }

    void setWindow(int window)
    {
        m_txWindow = window;
    }

    void setBuffer(void *buffer, int size)
    {
        m_buffer = reinterpret_cast<uint8_t *>(buffer);
        m_bufferSize = size;
    }

protected:
    tiny_fd_handle_t m_handle = nullptr;

private:
    uint8_t *m_buffer = nullptr;
    int m_bufferSize = 0;
    int m_mtu = 64;
    uint8_t m_txWindow = 2;
    int m_sendTimeout = 100;
    hdlc_crc_t m_crc = HDLC_CRC_16;
};

} // namespace tinyproto
