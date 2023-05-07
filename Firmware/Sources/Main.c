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
#define MAIN_THERMISTOR_VOLTAGE_DIVIDER_VOLTAGE_MILLIVOLTS 4096UL
/** The thermistor voltage divider R1 value in ohms. */
#define MAIN_THERMISTOR_VOLTAGE_DIVIDER_FIXED_RESISTOR_OHMS 10000UL

/** How many samples to get to compute the average on. */
#define MAIN_THERMISTOR_VOLTAGE_SAMPLES_COUNT 4 // Make sure the unsigned short value can't overflow with the sum of all samples when changing this value

/** The resistance corresponding to the highest temperature threshold, when reached the compressor is started. */
#define MAIN_THERMISTOR_THRESHOLD_HIGHEST_TEMPERATURE_RESISTANCE_OHMS 33160 // Stands for -19°C on the Brandt UD2321 freezer
/** The resistance corresponding to the lowest temperature threshold, when reached the compressor is stopped. */
#define MAIN_THERMISTOR_THRESHOLD_LOWEST_TEMPERATURE_RESISTANCE_OHMS 45000 // Stands for -26°C on the Brandt UD2321 freezer

/** The GPIO pin that drives the power led. */
#define MAIN_PIN_SET_POWER_LED(Value) GPIObits.GP2 = Value
/** The GPIO pin that drives the compressor led. */
#define MAIN_PIN_SET_COMPRESSOR_LED(Value) GPIObits.GP5 = Value
/** The GPIO pin that drives the compressor relay. */
#define MAIN_PIN_SET_COMPRESSOR_RELAY(Value) GPIObits.GP4 = Value

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
	unsigned short Thermistor_Voltage_Mean;
	unsigned char i;
	unsigned long Thermistor_Resistance;

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
	// Make sure leds and relay will be off
	MAIN_PIN_SET_POWER_LED(0);
	MAIN_PIN_SET_COMPRESSOR_LED(0);
	MAIN_PIN_SET_COMPRESSOR_RELAY(0);
	// Configure leds and relay pins as output
	TRISIO &= 0xCB;

	// Everything is initialized, turn on power led
	MAIN_PIN_SET_POWER_LED(1);
	// Blink compressor led 3 times to tell that everything is OK
	for (i = 0; i < 3; i++)
	{
		MAIN_PIN_SET_COMPRESSOR_LED(1);
		__delay_ms(250);
		MAIN_PIN_SET_COMPRESSOR_LED(0);
		__delay_ms(250);
	}

	// Main loop
	while (1)
	{
		// Get all samples with a little delay between each sample
		Thermistor_Voltage_Mean = 0;
		for (i = 0; i < MAIN_THERMISTOR_VOLTAGE_SAMPLES_COUNT; i++)
		{
			Thermistor_Voltage_Mean += ADCSampleFridgeTemperatureVoltage();
			__delay_ms(1000);
		}
		// Compute the moving average
		Thermistor_Voltage_Mean /= MAIN_THERMISTOR_VOLTAGE_SAMPLES_COUNT;

		// Use simple thresholds with hysteresis to drive the compressor
		Thermistor_Resistance = MainComputeThermistorResistance(Thermistor_Voltage_Mean);
		// The thermistor is a CTN on the UD2321, so resistance decreases when temperature increases
		if (Thermistor_Resistance <= MAIN_THERMISTOR_THRESHOLD_HIGHEST_TEMPERATURE_RESISTANCE_OHMS)
		{
			MAIN_PIN_SET_COMPRESSOR_RELAY(1);
			MAIN_PIN_SET_COMPRESSOR_LED(1);
		}
		else if (Thermistor_Resistance >= MAIN_THERMISTOR_THRESHOLD_LOWEST_TEMPERATURE_RESISTANCE_OHMS)
		{
			MAIN_PIN_SET_COMPRESSOR_RELAY(0);
			MAIN_PIN_SET_COMPRESSOR_LED(0);
		}
	}
}
