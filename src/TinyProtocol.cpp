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

#include "TinyProtocol.h"

namespace tinyproto
{

enum
{
    PROTO_RX_MESSAGE = 1,
    PROTO_RX_QUEUE_FREE = 1,
};

Proto::Proto(bool multithread)
   : m_link(nullptr)
   , m_multithread( multithread )
{
    tiny_events_create( &m_events );
}

Proto::~Proto()
{
    tiny_events_destroy( &m_events );
}

void Proto::setLink(ILinkLayer &link)
{
    m_link = &link;
}

ILinkLayer &Proto::getLink()
{
    return *m_link;
}

bool Proto::begin()
{
    tiny_events_set( &m_events, PROTO_RX_QUEUE_FREE );
    uint32_t timeout = m_link->getTimeout();
    if ( m_multithread && !timeout )
    {
        timeout = 100;
    }
    else if ( !m_multithread)
    {
        timeout = 0;
    }
    m_link->setTimeout( timeout );
    return m_link->begin(onReadCb, onSendCb, this);
}

bool Proto::send(const IPacket &packet, uint32_t timeout)
{
    bool result = false;
    uint32_t startTs = tiny_millis();
    for ( ;; )
    {
        result = m_link->put( packet.m_buf, packet.m_len );
        if ( result || timeout <= 0 )
        {
            break;
        }
        if ( static_cast<uint32_t>(tiny_millis() - startTs) >= timeout )
        {
            break;
        }
        if ( !m_multithread )
        {
            m_link->runTx();
            m_link->runRx();
        }
    }
    return result;
}

bool Proto::read(IPacket &packet, uint32_t timeout)
{
    int result = TINY_ERR_FAILED;
    uint32_t startTs = tiny_millis();
    for ( ;; )
    {
        uint8_t bits = tiny_events_wait(&m_events, PROTO_RX_MESSAGE, EVENT_BITS_CLEAR, m_multithread ? timeout : 0);
        if ( bits & PROTO_RX_MESSAGE )
        {
            if ( packet.m_buf )
            {
                memcpy( packet.m_buf, m_message, m_messageLen );
            }
            else
            {
                packet.m_buf = m_message;
                packet.m_size = m_link->getMtu();
            }
            packet.m_len = m_messageLen;
            packet.m_p = 0;
            tiny_events_set( &m_events, PROTO_RX_QUEUE_FREE );
            result = TINY_SUCCESS;
            break;
        }
        if ( static_cast<uint32_t>(tiny_millis() - startTs) >= timeout )
        {
            break;
        }
        if ( !m_multithread )
        {
            m_link->runTx();
            m_link->runRx();
        }
    }
    return result == TINY_SUCCESS;
}

void Proto::end()
{
    m_link->end();
    return;
}

void Proto::onRead(uint8_t *buf, int len)
{
    uint8_t bits = tiny_events_wait(&m_events, PROTO_RX_QUEUE_FREE, EVENT_BITS_CLEAR, 0);
    if ( bits == 0 )
    {
        // TODO: Lost frame
//        printf("#################################################### LOST\n");
    }
    m_message = buf;
    m_messageLen = len;
    tiny_events_set( &m_events, PROTO_RX_MESSAGE );
}

void Proto::onSend(uint8_t *buf, int len)
{
}

void Proto::onReadCb(void *udata, uint8_t *buf, int len)
{
    Proto *proto = reinterpret_cast<Proto *>(udata);
    proto->onRead(buf, len);
}

void Proto::onSendCb(void *udata, uint8_t *buf, int len)
{
    Proto *proto = reinterpret_cast<Proto *>(udata);
    proto->onSend(buf, len);
}


#if defined(ARDUINO)

#else

SerialFdProto::SerialFdProto(char *dev, bool multithread)
    : Proto( multithread )
    , m_layer( dev )
{
    setLink( m_layer );
}


SerialFdLink &SerialFdProto::getLink()
{
    return m_layer;
}

#endif

} // namespace tinyproto
