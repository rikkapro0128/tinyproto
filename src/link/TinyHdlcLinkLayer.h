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
#include "proto/hdlc/low_level/hdlc.h"

#include "hal/tiny_types.h"

#include <stdint.h>

namespace tinyproto
{

class IHdlcLinkLayer: public ILinkLayer
{
public:
    IHdlcLinkLayer(void *buffer, int size);

    ~IHdlcLinkLayer();

    bool begin(on_frame_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata) override;

    void end() override;

    bool put(void *buf, int size, uint32_t timeout) override;

    hdlc_crc_t getCrc()
    {
        return m_crc;
    }

    void setCrc( hdlc_crc_t crc )
    {
        m_crc = crc;
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
    hdlc_ll_handle_t m_handle = nullptr;
    void *m_udata;
    on_frame_cb_t m_onReadCb = nullptr;
    on_frame_send_cb_t m_onSendCb = nullptr;
    tiny_mutex_t  m_sendMutex{};
    tiny_events_t m_events{};
    void *m_tempBuffer = nullptr;

    uint8_t *m_buffer = nullptr;
    int m_bufferSize = 0;
    hdlc_crc_t m_crc = HDLC_CRC_8;

    static void onSend(void *udata, const uint8_t *data, int len);
    static void onRead(void *udata, uint8_t *data, int len);
};

} // namespace tinyproto
