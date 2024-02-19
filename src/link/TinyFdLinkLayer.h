/*
    Copyright 2016-2022 (C) Alexey Dynda

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
#include "proto/fd/tiny_fd.h"

#include <stdint.h>
#include <limits.h>

namespace tinyproto
{

class IFdLinkLayer: public ILinkLayer
{
public:
    IFdLinkLayer(void *buffer, int size);

    ~IFdLinkLayer();

    bool begin(on_frame_read_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata) override;

    void end() override;

    bool put(void *buf, int size, uint32_t timeout) override;

    void flushTx() override;

    int getWindow()
    {
        return m_txWindow;
    }

    hdlc_crc_t getCrc()
    {
        return m_crc;
    }

    void setCrc( hdlc_crc_t crc ) { m_crc = crc; }

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

    int parseData(const uint8_t *data, int size);

    int getData(uint8_t *data, int size);

private:
    tiny_fd_handle_t m_handle = nullptr;
    uint8_t *m_buffer = nullptr;
    int m_bufferSize = 0;
    uint8_t m_txWindow = 2;
    hdlc_crc_t m_crc = HDLC_CRC_8;
};

} // namespace tinyproto
