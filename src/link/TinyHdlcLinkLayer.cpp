/*
    Copyright 2021 (C) Alexey Dynda

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

#include "TinyHdlcLinkLayer.h"

namespace tinyproto
{

IHdlcLinkLayer::IHdlcLinkLayer(void *buffer, int size)
    : m_buffer(reinterpret_cast<uint8_t *>(buffer))
    , m_bufferSize(size)
{
}

IHdlcLinkLayer::~IHdlcLinkLayer()
{
}

bool IHdlcLinkLayer::begin(on_frame_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata)
{
    hdlc_ll_init_t init{};
    init.user_data = udata;
    init.on_frame_read = onReadCb;
    init.on_frame_send = onSendCb;
    init.buf = m_buffer;
    init.buf_size = m_bufferSize;
    init.crc_type = getCrc();
    init.mtu = getMtu();
    int result = hdlc_ll_init(&m_handle, &init);
    return result == TINY_SUCCESS;
}

void IHdlcLinkLayer::end()
{
    hdlc_ll_close(m_handle);
    m_handle = nullptr;
}

bool IHdlcLinkLayer::put(void *buf, int size)
{
    return hdlc_ll_put(m_handle, buf, size) >= 0;
}

int IHdlcLinkLayer::parseData(const uint8_t *data, int size)
{
    return hdlc_ll_run_rx(m_handle, data, size, nullptr);
}

int IHdlcLinkLayer::getData(uint8_t *data, int size)
{
    return hdlc_ll_run_tx(m_handle, data, size);
}

/////////////////////////////////////////////////////////////////////////////

} // namespace tinyproto
