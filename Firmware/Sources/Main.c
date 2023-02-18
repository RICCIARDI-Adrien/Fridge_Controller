/** @file Main.c
 * Entry point and main loop of the fridge controller firmware.
 * @author Adrien RICCIARDI
 */
#include <xc.h>

//-------------------------------------------------------------------------------------------------
// Microcontroller configuration
//-------------------------------------------------------------------------------------------------
// CONFIG register
#pragma config FCMEN = OFF, IESO = OFF, BOREN = ON, CPD = OFF, CP = OFF, MCLRE = ON, PWRTE = ON, WDTE = OFF, FOSC = INTOSCIO // Disable Fail-Safe Clock Monitor, disable Internal External clock Switchover, enable Brown Out Detection, disable Data Code Protection, disable Code Protection, enable MCLR pin, enable Power-up Timer, disable Watchdog Timer, select the internal oscillator and keep the oscillator pins as GPIOs

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
void main(void)
{
	// Clock the chip from the internal oscillator at 1MHz
	OSCCON = 0x41;
	while (!OSCCONbits.HTS); // Wait for the clock to be stable, even if it already should be

	// Disable comparator analog mode on all pins
	CMCON0 = 0x07;
	
	ANSELbits.ANS0 = 0;
	GPIO = 0;
	TRISIO = 0x3E;

	while (1)
	{
		GPIObits.GP0 = 1;
		__delay_ms(1000);
		GPIObits.GP0 = 0;
		__delay_ms(1000);
	}
}
