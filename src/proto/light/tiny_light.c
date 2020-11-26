/*
    Copyright 2017-2020 (C) Alexey Dynda

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

#include "tiny_light.h"
#include "proto/hdlc2/tiny_hdlc2_int.h"
#include <stddef.h>

#ifdef CONFIG_ENABLE_STATS
    #define STATS(x) x
#else
    #define STATS(x)
#endif

/************************************************************
*
*  INTERNAL FRAME STRUCTURE
*
*   8     any len       8
* | 7E |  USER DATA  | 7E |
*
* 7E is standard frame delimiter (commonly use on layer 2)
* FCS is standard checksum. Refer to RFC1662 for example.
* UID is reserved for future usage
*************************************************************/

#define FLAG_SEQUENCE            0x7E
#define TINY_ESCAPE_CHAR         0x7D
#define TINY_ESCAPE_BIT          0x20

//////////////////////////////////////////////////////////////////////////////

/**************************************************************
*
*                 OPEN/CLOSE FUNCTIONS
*
***************************************************************/

static int on_frame_read(void *user_data, void *data, int len);
static int on_frame_sent(void *user_data, const void *data, int len);

int tiny_light_init(STinyLightData *handle,
                    write_block_cb_t write_func,
                    read_block_cb_t read_func,
                    void *pdata)
{
    if (!handle || !write_func || !read_func)
    {
        return TINY_ERR_FAILED;
    }
    tiny_hdlc_init_t init={};
    init.user_data = handle;
    init.on_frame_read = on_frame_read;
    init.on_frame_sent = on_frame_sent;
    init.buf = &handle->buffer[0];
    init.buf_size = LIGHT_BUF_SIZE;
    init.crc_type = ((STinyLightData *)handle)->crc_type;

    handle->user_data = pdata;
    handle->read_func = read_func;
    handle->write_func = write_func;

    return tiny_hdlc_init( &handle->_hdlc, &init );
}

//////////////////////////////////////////////////////////////////////////////

int tiny_light_close(STinyLightData *handle)
{
    if (!handle)
    {
        return TINY_ERR_FAILED;
    }
    tiny_hdlc_close( handle->_hdlc );
    return TINY_SUCCESS;
}

/**************************************************************
*
*                 SEND FUNCTIONS
*
***************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////

static int on_frame_sent(void *user_data, const void *data, int len)
{
    return len;
}

/////////////////////////////////////////////////////////////////////////////////////////

int tiny_light_send(STinyLightData *handle, const uint8_t * pbuf, int len)
{
    int result = TINY_SUCCESS;
    tiny_hdlc_put( handle->_hdlc, pbuf, len );
    while ( handle->_hdlc->tx.origin_data )
    {
        uint8_t stream[1];
        int stream_len = tiny_hdlc_run_tx( handle->_hdlc, stream, sizeof(stream) );
        do
        {
            result = handle->write_func( handle->user_data, stream, stream_len );
            if ( result < 0 )
            {
                return result;
            }
        } while (result < stream_len);
    }
    return result >= 0 ? len: result;
}

//////////////////////////////////////////////////////////////////////////////////////

/**************************************************************
*
*                 RECEIVE FUNCTIONS
*
***************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////

static int on_frame_read(void *user_data, void *data, int len)
{
    STinyLightData *handle = (STinyLightData *)user_data;
    handle->rx_len = len;
    return len;
}

/////////////////////////////////////////////////////////////////////////////////////////

int tiny_light_read(STinyLightData *handle, uint8_t *pbuf, int len)
{
    uint32_t ts = tiny_millis();
    int result = 0;
    handle->_hdlc->rx_buf = pbuf;
    handle->_hdlc->rx_buf_size = len;
    handle->rx_len = 0;
    do
    {
        uint8_t stream[1];
        int stream_len = handle->read_func( handle->user_data, stream, sizeof(stream) );
        if ( stream_len < 0 )
        {
            result = stream_len;
            break;
        }
        tiny_hdlc_run_rx( handle->_hdlc, stream, stream_len, &result );
        if ( result == TINY_SUCCESS )
        {
            tiny_hdlc_run_rx( handle->_hdlc, stream, 0, &result );
        }
        if ( result != TINY_SUCCESS )
        {
            break;
        }
        if ( (uint32_t)(tiny_millis() - ts) > 1000 )
        {
            break;
        }
    } while ( handle->rx_len == 0 );
    if ( handle->rx_len != 0 )
    {
        result = handle->rx_len;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

tiny_hdlc_handle_t tiny_light_get_hdlc(STinyLightData *handle)
{
    return handle->_hdlc;
}

/////////////////////////////////////////////////////////////////////////////////////////
