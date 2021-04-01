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

class ILinkLayer
{
public:
    /**
     *
     */
    virtual bool begin(on_frame_cb_t onReadCb, on_frame_cb_t onSendCb, void *udata) = 0;

    virtual void end() = 0;

    virtual void runRx() = 0;

    virtual void runTx() = 0;

    virtual bool put(void *buf, int size) = 0;

    void setTimeout(uint32_t timeout)
    {
        m_timeout = timeout;
    }

    uint32_t getTimeout()
    {
        return m_timeout;
    }

    int getMtu()
    {
        return m_mtu;
    }

    void setMtu(int mtu)
    {
        m_mtu = mtu;
    }

private:
    int m_mtu = 16384;
    uint32_t m_timeout;
};

} // namespace tinyproto
