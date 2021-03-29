/*
    MIT License

    Copyright (c) 2021, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <sdkconfig.h>
#include <stdio.h>


#define BUFSIZE  128

#if defined(CONFIG_TARGET_PLATFORM_ESP8266) || defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP8266) || defined(__ESP8266__)
#define ESP8266_DETECTED
#endif

#if defined(ESP8266_DETECTED)
#ifndef UART_PIN_NO_CHANGE
#define UART_PIN_NO_CHANGE (-1)
#endif
#endif

void tiny_serial_close(tiny_serial_handle_t port)
{
    uart_driver_delete( port );
}

tiny_serial_handle_t tiny_serial_open(const char *name, uint32_t baud)
{
    tiny_serial_handle_t handle = -1;
    if ( !strcmp(name, "uart0") )
    {
        handle = UART_NUM_0;
    }
    else if ( !strcmp(name, "uart1") )
    {
        handle = UART_NUM_1;
    }
#if !defined(ESP8266_DETECTED)
    else if ( !strcmp(name, "uart2") )
    {
        handle = UART_NUM_2;
    }
#endif
    if ( handle < 0 )
    {
        return TINY_SERIAL_INVALID;
    }

    int tx_gpio = UART_PIN_NO_CHANGE;
    int rx_gpio = UART_PIN_NO_CHANGE;
    int rts_gpio = UART_PIN_NO_CHANGE;
    int cts_gpio = UART_PIN_NO_CHANGE;

    uart_config_t uart_config = {
        .baud_rate = baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
#if !defined(ESP8266_DETECTED)
        .use_ref_tick = false,
        //        .source_clk = 0, // APB
#endif
    };

    char *token = strtok( dev, "," );
    int index = 0;
    while (token)
    {
        int value = strtol( token, NULL, 10 );
        switch (index)
        {
        case 0: tx_gpio = value; break;
        case 1: rx_gpio = value; break;
        case 2: rts_gpio = value; break;
        case 3: cts_gpio = value; break;
        default: break;
        }
        index++;
        token = strtok( NULL, "," );
    }

    ESP_ERROR_CHECK(uart_param_config(handle, &uart_config));
    ESP_ERROR_CHECK( uart_set_pin(handle, tx_gpio, rx_gpio, rts_gpio, cts_gpio));
    //                                  uart,   rx size,     tx size, event queue size, queue, flags
    ESP_ERROR_CHECK(uart_driver_install(handle, BUFSIZE,     0,       0,                NULL,  0));

    return handle;
}

int tiny_serial_send(tiny_serial_handle_t port, const void *buf, int len)
{
    return tiny_serial_send_timeout( port, buf, len, 100 );
}

int tiny_serial_send_timeout(tiny_serial_handle_t port, const void *buf, int len, uint32_t timeout_ms)
{
    const uint8_t *ptr = (const uint8_t *)buf;
    int sent = 0;
    TickType_t start_ticks = xTaskGetTickCount();
    TickType_t ticks_left = (timeout_ms + portTICK_PERIOD_MS / 2) / portTICK_PERIOD_MS;
    while ( sent < len )
    {
         int n = uart_tx_chars( port, (const char *)ptr, len - sent );
         if ( n < 0 )
         {
             break;
         }
         sent += n;
         ptr += n;
         if ( sent >= len || ticks_left <= 0 )
         {
             break;
         }
         uart_wait_tx_done( port, ticks_left ); 
         TickType_t delta = (TickType_t)(xTaskGetTickCount() - start_ticks);
         ticks_left = delta < ticks_left ? (ticks_left - delta) : 0;
    }
//    return uart_write_bytes(port, (const char *)b, s); }
    return sent;
}

int tiny_serial_read(tiny_serial_handle_t port, void *buf, int len)
{
    return tiny_serial_read_timeout( port, buf, len, 100 );
}

int tiny_serial_read_timeout(tiny_serial_handle_t port, void *buf, int len, uint32_t timeout_ms)
{
    return uart_read_bytes(port, (uint8_t *)b, s, timeout_ms / portTICK_PERIOD_MS); }
}
