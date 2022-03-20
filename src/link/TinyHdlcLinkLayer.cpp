/*
    Copyright 2021-2022 (C) Alexey Dynda

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

enum
{
    TX_QUEUE_FREE = 1,
    TX_MESSAGE_SENT = 2,
};

IHdlcLinkLayer::IHdlcLinkLayer(void *buffer, int size)
    : m_buffer(reinterpret_cast<uint8_t *>(buffer))
    , m_bufferSize(size)
{
    tiny_mutex_create( &m_sendMutex );
    tiny_events_create( &m_events );
    tiny_events_set( &m_events, TX_QUEUE_FREE );
}

IHdlcLinkLayer::~IHdlcLinkLayer()
{
    tiny_events_destroy( &m_events );
    tiny_mutex_destroy( &m_sendMutex );
}

bool IHdlcLinkLayer::begin(on_frame_read_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata)
{
    hdlc_ll_init_t init{};
    m_onReadCb = onReadCb;
    m_onSendCb = onSendCb;
    m_udata = udata;
    init.user_data = this;
    init.on_frame_read = onRead;
    init.on_frame_send = onSend;
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

bool IHdlcLinkLayer::put(void *buf, int size, uint32_t timeout)
{
    bool result = false;
    uint8_t bits = tiny_events_wait( &m_events, TX_QUEUE_FREE, EVENT_BITS_CLEAR, timeout );
    if ( bits )
    {
        tiny_mutex_lock( &m_sendMutex );
        if ( m_tempBuffer == buf )
        {
            m_tempBuffer = nullptr;
            tiny_events_set( &m_events, TX_QUEUE_FREE );
            result = true;
        }
        else if ( hdlc_ll_put(m_handle, buf, size) == TINY_SUCCESS )
        {
            m_tempBuffer = buf;
        }
        else
        {
            tiny_events_set( &m_events, TX_QUEUE_FREE );
        }
        tiny_mutex_unlock( &m_sendMutex );
    }
    return result;
}

void IHdlcLinkLayer::flushTx()
{
    tiny_mutex_lock( &m_sendMutex );
    hdlc_ll_reset( m_handle, HDLC_LL_RESET_TX_ONLY );
    tiny_mutex_unlock( &m_sendMutex );
    tiny_events_set( &m_events, TX_QUEUE_FREE );
}

int IHdlcLinkLayer::parseData(const uint8_t *data, int size)
{
    return hdlc_ll_run_rx(m_handle, data, size, nullptr);
}

int IHdlcLinkLayer::getData(uint8_t *data, int size)
{
    if ( tiny_events_wait( &m_events, TX_QUEUE_FREE, EVENT_BITS_LEAVE, 0 ) != TX_QUEUE_FREE )
    {
        return hdlc_ll_run_tx(m_handle, data, size);
    }
    return 0;
}

void IHdlcLinkLayer::onSend(void *udata, const uint8_t *data, int len)
{
    IHdlcLinkLayer *layer = reinterpret_cast<IHdlcLinkLayer *>(udata);
    tiny_events_set( &layer->m_events, TX_QUEUE_FREE );
    layer->m_onSendCb( layer->m_udata, 0, data, len );
}

void IHdlcLinkLayer::onRead(void *udata, uint8_t *data, int len)
{
    IHdlcLinkLayer *layer = reinterpret_cast<IHdlcLinkLayer *>(udata);
    layer->m_onReadCb( layer->m_udata, 0, data, len );
}

/////////////////////////////////////////////////////////////////////////////

} // namespace tinyproto
