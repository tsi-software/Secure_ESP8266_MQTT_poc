
// F_CPU tells util/delay.h our clock frequency
#ifndef F_CPU
#define F_CPU 8000000UL // 8MHz
#endif


//#define DD_MISO   B,4
//#define DD_MOSI   B,3
//#define DD_SCK    B,5
//#define DD_SS     B,2


#include <avr/io.h>
//#include <stdlib.h>
//#include <util/delay.h>


void spi_init_slave( void )
{
    // MISO as OUTPUT
    DDRB = (1<<4);
    //pinMode(MISO, OUTPUT); //Arduino

    /*  Enable SPI.
        SPCR0:
        Bit 7 – SPIE0: SPI0 Interrupt Enable
        Bit 6 – SPE0: SPI0 Enable
        Bit 5 – DORD0: Data0 Order
            0 = the MSB of the data word is transmitted first.
            1 = the LSB of the data word is transmitted first.
        Bit 4 – MSTR0: Master/Slave0 Select
            0 = Slave
            1 = Master
        Bit 3 – CPOL0: Clock0 Polarity
            0 = SCK is low when idle
            1 = SCK is high when idle
        Bit 2 – CPHA0: Clock0 Phase
            0 = data is sampled on the leading edge of SCK
            1 = data is sampled on the trailing edge of SCK
        Bits 1:0 – SPR0n: SPI0 Clock Rate Select n [n = 1:0]
    */
    // MSB First,
    // SCK is low when idle,
    // data is sampled on the leading edge of SCK
    SPCR = (1<<SPE);
}


unsigned char spi_tranceiver( unsigned char data )
{
    // Load data to send.
    SPDR = data;

    // Wait until transmission complete.
    while(!( SPSR & (1<<SPIF) ));

    // Return received data.
    return(SPDR);
}


int main( void ) {
    spi_init_slave();

    // Port D: set all pins to HIGH, and to OUTPUT.
    PORTD = 0xFF;
    DDRD =  0xFF;

    while(1) {
        PORTD = spi_tranceiver( 0 );
    }
}
