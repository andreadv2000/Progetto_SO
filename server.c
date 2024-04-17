
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <uart.h>

//******************Setting Output Pins******************//

/*
 * WGM2 = 101 for fast PWM, 8-bit, TOP = 0xFF, update OCR2x at BOTTOM, TOV1 flag set on Top
 * WGM3 = 101 for fast PWM, 8-bit, TOP = 0xFF, update OCR3x at BOTTOM, TOV3 flag set on Top
 * WGM4 = 101 for fast PWM, 8-bit, TOP = 0xFF, update OCR4x at BOTTOM, TOV4 flag set on Top
 
  * When a timer is configured for PWM output, it continually counts from 0 up to a maximum value.
  * When the count matches a configured compare value,an action is triggered.
  * This action can either be to set, clear,or toggle the output pin 
*/
/*
  * CS2 = 011 for prescaler = 64 => cloack = 16MHz/64 = 250kHz
  * CS3 = 011 for prescaler = 64 => cloack = 16MHz/64 = 250kHz
  * CS4 = 011 for prescaler = 64 => cloack = 16MHz/64 = 250kHz
*/
/*
  * COM2A = 11 set OC2A on compare match(set Output to hight level), clear OC2A at BOTTOM,non-inverting mode(hight level on compare match)
  * COM3B = 11 set OC3B on compare match(set Output to hight level), clear OC3B at BOTTOM,non-inverting mode(hight level on compare match)
  * COM4C = 11 set OC4C on compare match(set Output to hight level), clear OC4C at BOTTOM,non-inverting mode(hight level on compare match)
*/
#define TCCR2A_MASK (1<<COM2A1)|(1<<COM2A0)|(1<<WGM20) // PIN 10
#define TCCR2B_MASK (1<<CS20)|(1<<CS21)|(1<<WGM22) // PIN 10
#define TCCR4A_MASK (1<<COM4A0)|(1<<COM4A1)|(1<<WGM40) //PIN 6 
#define TCCR4B_MASK (1<<CS40)|(1<<CS41)|(1<<WGM42) //PIN 6
#define TCCR3A_MASK (1<<COM3B0)|(1<<COM3B1)|(1<<WGM30) //PIN 2
#define TCCR3B_MASK (1<<CS30)|(1<<CS31)|(1<<WGM32) //PIN 2

//********************************************************/
int main(){
  
}