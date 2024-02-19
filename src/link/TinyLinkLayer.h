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
#include "proto/fd/tiny_fd.h"

#include <stdint.h>
#include <limits.h>

namespace tinyproto
{

/**
 * This is basic class for C++ Link Layer objects.
 */
class ILinkLayer
{
public:
     ILinkLayer() {}

    /**
     * The method initializes the link layer protocol, and connects custom callbacks
     * to link layer. Once the new frame is received, onReadCb will be called, once frame is sent onSendCb
     * will be called.
     *
     * @param onReadCb callback to call when a frame is received. Remember that processing must be as quick as possible
     * @param onSendCb callback to call when a frame is sent.
     * @param udata user defined data, will be passed to callbacks
     * @return true if successful, false is initialization error happened.
     */
    virtual bool begin(on_frame_read_cb_t onReadCb, on_frame_send_cb_t onSendCb, void *udata) = 0;

    /**
     * Stops link layer protocol
     */
    virtual void end() = 0;

    /**
     * Runs rx part of the protocol. This method is automatically called by the owner of the protocol (i.e. tinyproto::Proto)
     * The actual implementation on runRx() method depends on hardware channel used.
     */
    virtual void runRx() = 0;

    /**
     * Runs tx part of the protocol. This method is automatically called by the owner of the protocol (i.e. tinyproto::Proto)
     * The actual implementation on runTx() method depends on hardware channel used.
     */
    virtual void runTx() = 0;

    /**
     * Puts new data for sending over the link layer.
     *
     * @param buf pointer to the buffer with the data
     * @param size size of the data in the buffer
     * @param timeout timeout in milliseconds to wait for operation to complete
     * @return true if the data is successfully put to the queue, false if timeout happened or error
     */
    virtual bool put(void *buf, int size, uint32_t timeout) = 0;

    /**
     * Flush tx operation if possible
     */
    virtual void flushTx() = 0;

    /**
     * Sets timeout of Rx/Tx operations in milliseconds for the link layer protocol.
     * This is not the same timeout, as timeout used by put() method.
     *
     * @param timeout timeout in milliseconds for Tx/Rx operations
     */
    void setTimeout(uint32_t timeout)
    {
        m_timeout = timeout;
    }

    /**
     * Returns current timeout of Rx/Tx operations
     */
    uint32_t getTimeout()
    {
        return m_timeout;
    }

    /**
     * Returns current mtu for the link layer protocol in bytes.
     */
    int getMtu()
    {
        return m_mtu;
    }

    /**
     * Set protocol mtu (maximum transmission unit) payload.
     * Allowable value depends on the resources of the controller used, and actual low layer protocol.
     * mtu can be configured only before protocol initialization (before calling begin() method)
     *
     * @param mtu size in bytes
     */
    void setMtu(int mtu)
    {
        m_mtu = mtu;
    }

    /**
     * Default virtual destructor
     */
    virtual ~ILinkLayer() = default;

private:
    int m_mtu = 16384;
    uint32_t m_timeout = 0;
};

} // namespace tinyproto
