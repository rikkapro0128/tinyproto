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

#include "TinyFdLinkLayer.h"

namespace tinyproto
{

IFdLinkLayer::IFdLinkLayer(void *buffer, int size)
    : m_buffer(reinterpret_cast<uint8_t *>(buffer))
    , m_bufferSize(size)
{
}

IFdLinkLayer::~IFdLinkLayer()
{
}

bool IFdLinkLayer::begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata)
{
    tiny_fd_init_t init{};
    init.pdata = udata;
    init.on_frame_cb = onReadCb;
    init.on_sent_cb = onSendCb;
    init.buffer = m_buffer;
    init.buffer_size = m_bufferSize;
    init.window_frames = m_txWindow;
    init.send_timeout = getTimeout();
    init.retry_timeout = getTimeout() / 4;
    init.retries = 2;
    init.crc_type = getCrc();
    init.mtu = getMtu();
    int result = tiny_fd_init(&m_handle, &init);
    return result == TINY_SUCCESS;
}

void IFdLinkLayer::end()
{
    tiny_fd_close(m_handle);
    m_handle = nullptr;
}

bool IFdLinkLayer::put(void *buf, int size)
{
    return tiny_fd_send_packet(m_handle, buf, size) >= 0;
}

int IFdLinkLayer::parseData(const uint8_t *data, int size)
{
    return tiny_fd_on_rx_data(m_handle, data, size);
}

int IFdLinkLayer::getData(uint8_t *data, int size)
{
    return tiny_fd_get_tx_data(m_handle, data, size);
}

/////////////////////////////////////////////////////////////////////////////

} // namespace tinyproto
