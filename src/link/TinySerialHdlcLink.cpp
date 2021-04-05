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

#include "TinySerialHdlcLink.h"
#include <stdlib.h>

namespace tinyproto
{

SerialHdlcLink::~SerialHdlcLink()
{
    if ( m_buffer )
    {
        free(m_buffer);
        m_buffer = nullptr;
    }
}

bool SerialHdlcLink::begin(on_frame_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata)
{
    int size = hdlc_ll_get_buf_size_ex(getMtu(), getCrc(), 3);
    m_buffer = reinterpret_cast<uint8_t *>(malloc(size));
    setBuffer(m_buffer, size);
    return ISerialLinkLayer<IHdlcLinkLayer,128>::begin(onReadCb, onSendCb, udata);
}

void SerialHdlcLink::end()
{
    ISerialLinkLayer<IHdlcLinkLayer,128>::end();
    if ( m_buffer )
    {
        free(m_buffer);
        m_buffer = nullptr;
    }
}

} // namespace tinyproto
