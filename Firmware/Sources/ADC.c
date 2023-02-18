/** @file ADC.c
 * See ADC.h for description.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <xc.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void ADCInitialize(void)
{
	// Set the ADC conversion clock to Fosc/4, it allows to complete a conversion in 4us with Fosc = 1MHz (this is slow enough to reach a correct ADC setup time and fast enough in this application)
	ANSEL &= 0x0F;
	ANSEL |= 0x40;

	// Configure the ADC module
	ADCON0 = 0xC1; // Enable right justification of the sampling result, use VREF pin as reference voltage, select channel 0 and enable the ADC module
}

unsigned short ADCSampleFridgeTemperatureVoltage(void)
{
	unsigned short Result;

	// Start conversion
	ADCON0bits.GO = 1;

	// Wait for the conversion to terminate
	while (ADCON0bits.GO);

	// Retrieve the sampled voltage
	Result = (ADRESH << 8) | ADRESL;
	Result &= 0x03FF; // Bits 15 to 10 are undefined, so force them to zero

	return Result;
}
