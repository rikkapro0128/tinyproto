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

#include "TinyLinkLayer.h"

namespace tinyproto
{

IFdLinkLayer::IFdLinkLayer(void *buffer, int size, int mtu, int window)
   : m_buffer( reinterpret_cast<uint8_t *>(buffer) )
   , m_bufferSize( size )
   , m_mtu( mtu )
   , m_window( window )
{
}

void IFdLinkLayer::begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata)
{
    tiny_fd_init_t init{};
    init.pdata = udata;
    init.on_frame_cb = onReadCb;
    init.on_sent_cb = onSendCb;
    init.buffer = m_buffer;
    init.buffer_size = m_bufferSize;
    init.window_frames = m_window;
    init.send_timeout = m_sendTimeout;
    init.retry_timeout = 200;
    init.retries = 2;
    init.crc_type = m_crc;
    init.mtu = getMtu();

    tiny_fd_init(&m_handle, &init);
}

void IFdLinkLayer::end()
{
    tiny_fd_close(m_handle);
    m_handle = nullptr;
}


}

