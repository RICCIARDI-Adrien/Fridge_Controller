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
// Private constants
//-------------------------------------------------------------------------------------------------
/** The thermistor voltage divider voltage in millivolts. */
#define MAIN_THERMISTOR_VOLTAGE_DIVIDER_VOLTAGE_MILLIVOLTS 4000UL
/** The thermistor voltage divider R1 value in ohms. */
#define MAIN_THERMISTOR_VOLTAGE_DIVIDER_FIXED_RESISTOR_OHMS 10000UL

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Compute the fridge thermistor resistance.
 * @param Sampled_Voltage The thermistor voltage in range [0; 1023].
 * @return The thermistor resistance in ohms.
 */
static inline unsigned long MainComputeThermistorResistance(unsigned short Sampled_Voltage)
{
	unsigned long Voltage_Millivolts, Thermistor_Resistance;

	// Convert the sampled voltage to millivolts
	Voltage_Millivolts = (MAIN_THERMISTOR_VOLTAGE_DIVIDER_VOLTAGE_MILLIVOLTS * Sampled_Voltage) / 1023UL; // The maximum sampled value (1023) is equal to the voltage divider voltage, the maximum value for the multiplication is 4000*1023 and can't overflow an unsigned 32-bit integer

	// Retrieve the R2 value of the voltage divider bridge with the following formula : R2 = (U2 * R1) / (U - U2)
	Thermistor_Resistance = Voltage_Millivolts * MAIN_THERMISTOR_VOLTAGE_DIVIDER_FIXED_RESISTOR_OHMS; // The maximum value for the multiplication is 4000*10000 and can't overflow an unsigned 32-bit integer
	if (Voltage_Millivolts >= MAIN_THERMISTOR_VOLTAGE_DIVIDER_VOLTAGE_MILLIVOLTS) Voltage_Millivolts = MAIN_THERMISTOR_VOLTAGE_DIVIDER_VOLTAGE_MILLIVOLTS - 1; // Handle negative values (even if they should not occur) and division by zero
	Thermistor_Resistance /= MAIN_THERMISTOR_VOLTAGE_DIVIDER_VOLTAGE_MILLIVOLTS - Voltage_Millivolts;

	return Thermistor_Resistance;
}

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
		GPIObits.GP5 = 0;
		__delay_ms(500);
		GPIObits.GP2 = 0;
		GPIObits.GP5 = 1;
		__delay_ms(500);
	}*/
	
	while (1)
	{
		unsigned short test;
		unsigned long test2;

		test = ADCSampleFridgeTemperatureVoltage();
		test2 = MainComputeThermistorResistance(test);
		if (test2 > 10000) GPIObits.GP2 = 1;
		else GPIObits.GP2 = 0;

		__delay_ms(200);
		GPIObits.GP5 = !GPIObits.GP5;
	}
}
