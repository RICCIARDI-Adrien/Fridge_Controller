/** @file ADC.h
 * Sample analog voltages thanks to the analog-to-digital module.
 * @author Adrien RICCIARDI
 */
#ifndef H_ADC_H
#define H_ADC_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Configure the ADC module to use the external precision reference voltages. */
void ADCInitialize(void);

/** Read the raw voltage (0 = 0V, 1023 = 4V) on the fridge thermistor.
 * @return The voltage value in range [0, 1023].
 */
unsigned short ADCSampleFridgeTemperatureVoltage(void);

#endif
