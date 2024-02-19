/*
 * This example sends back every buffer received over UART.
 *
 * !README!
 * The sketch is developed to perform UART tests between Arduino
 * and PC.
 * 1. Burn this program to Arduino
 * 2. Compile tiny_loopback tool (see tools folder) for your system
 * 3. Connect Arduino TX and RX lines to your PC com port
 * 4. Run tiny_loopback on the PC (use correct port name on your system)
 * 5. tiny_loopback will print the test speed results
 *
 * Also, this example demonstrates how to pass data between 2 systems
 * By default the sketch and tiny_loopback works as 115200 speed.
 */
#include <TinyProtocol.h>

// By default SerialFdProto class allows sending and receiving
// messages upto 32 bytes, we will use Serial0
tinyproto::SerialFdProto proto(SerialUSB);

void setup()
{
    /* Run at 115200 */
    proto.getLink().setSpeed( 115200 );
    /* Lets use 8-bit checksum, available on all platforms */
    proto.getLink().setCrc( HDLC_CRC_8 ); //enableCheckSum();
    /* Start */
    proto.begin();
}

void loop()
{
    tinyproto::StaticPacket<32> packet;
    if ( proto.read(packet, 0) )
    {
        // process received packet
    }
}
