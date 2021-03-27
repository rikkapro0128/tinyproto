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

static tiny_fd_handle_t handle = nullptr;

Proto::Proto(ILinkLayer &link, bool multithread)
{
// Memory allocation
//     1. RX queue
//     2. TX queue
//     3. Low level protocol
//    timeout = 0 for single
//              xx for multi
}

Proto::~Proto()
{
}


bool Proto::begin(ILinkLayer &link)
{
    m_link = &link;
    // TODO: Callbacks
//    m_link->setCallback( );
    m_link->begin();
    return true;
}

#if defined(ARDUINO)
bool Proto::beginSerial()
{
    return true;
}
#elif defined(__linux__) || defined(_WIN32)
bool Proto::beginSerial(char *device)
{
    return false;
}
#endif

bool Proto::send(const IPacket &packet, int timeout)
{
    int result;
    for (;;)
    {
        result = tiny_fd_send_packet( handle, packet.m_buf, packet.m_len );
        if ( result == TINY_SUCCESS || timeout <= 0 )
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

bool Proto::read(IPacket &packet, int timeout)
{
    int result;
    for (;;)
    {
        int bits = tiny_events_wait( &m_events, 1, EVENT_BITS_CLEAR, m_multithread ? timeout: 0);
        if ( bits & 1 )
        {
            // TODO: Read from queue
            result = TINY_SUCCESS;
            break;
        }
        if ( timeout <= 0 )
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

void Proto::init()
{
    uint8_t *ptr = m_buffer;
    m_rxQueue = reinterpret_cast<IPacket *>(ptr);
    ptr += sizeof(IPacket) * m_rxQueueSize;
    for (int i = 0; i < m_rxQueueSize; i++)
    {
        m_rxQueue[i] = IPacket( (char *)ptr, m_link->getMtu() );
        ptr += m_link->getMtu();
    }
}

}

