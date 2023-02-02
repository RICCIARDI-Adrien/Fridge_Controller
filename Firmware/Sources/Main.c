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
	while (1);
}
