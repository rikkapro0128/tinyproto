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

#pragma once

#include "hal/tiny_serial.h"

#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include <stdint.h>
#include <limits.h>

namespace tinyproto
{

class Serial
{
public:
#if defined(ARDUINO)
    Serial(HardwareSerial &dev);
#endif
    Serial(const char *dev);

    void setTimeout(uint32_t timeoutMs);

    bool begin(int speed);

    void end();

    int readBytes(uint8_t *buf, int len);

    int write(const uint8_t *buf, int len);

private:
    const char *m_dev;
    tiny_serial_handle_t m_handle = -1;
    uint32_t m_timeoutMs = 0;
};

} // namespace tinyproto
