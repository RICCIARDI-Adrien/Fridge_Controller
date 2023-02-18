/** @file Main.c
 * Entry point and main loop of the fridge controller firmware.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
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

	// Configure modules
	ADCInitialize();

	// Configure the pins now that all modules are initialized
	CMCON0 = 0x07; // Disable the comparator analog mode on all pins as comparator is not used
	// Configure GP0 and GP1 as analog inputs and other pins as digital
	ANSEL &= 0xF0;
	ANSEL |= 0x03;

	// TEST
	GPIObits.GP2 = 0;
	GPIObits.GP5 = 0;
	TRISIObits.TRISIO2 = 0;
	TRISIObits.TRISIO5 = 0;

	/*while (1)
	{
		GPIObits.GP2 = 1;
		__delay_ms(1000);
		GPIObits.GP2 = 0;
		__delay_ms(1000);
	}*/
	
	while (1)
	{
		unsigned short test;

		test = ADCSampleFridgeTemperatureVoltage();
		if (test > 255) GPIObits.GP2 = 1;
		else GPIObits.GP2 = 0;
		
		//__delay_ms(1000);
		GPIObits.GP5 = !GPIObits.GP5;
	}
}
