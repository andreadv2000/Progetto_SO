
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

//******************Inizialize Volatile Variables******************//
volatile int interrupt_occurred = 0;
volatile int interrupt_counter = 0;
//********************************************************/

int ret, idx;
float sampling_time;

char sent_message[2024];
float ADC_values[3] = {0.0,0.0,0.0};


//******************Interrupt Service Routine******************//
ISR(TIMER5_COMPA_vect){
    interrupt_occurred = 1;
    interrupt_counter++;
}

//********************************************************/

void ADC_init(){

    /* Clear the ADMUX register */
    ADMUX = 0;

    /* Clear the ADCSRA register */
    ADCSRA = 0;

    /*
     * ADMUX Register Description:
     * Bit 7 – REFS1:0: Aref Select
       REFS1:0 is the AREF select bit. This bit selects the voltage reference for the analog-to-digital converter (ADC).
       *  00 = AREF,	External Reference on PORT A (default)
       *  01 = AVCC with external capacitor on REF pin
       *  10 = Internal 2.56V Voltage Reference with external capacitor on REF pin
       *  11 = Reserved
     * Bit 6 – ADLAR: Right Adjust Data
       When this bit is set, the ADC result is right justified.
     * Bit 5 – MUX3:0: ADC Multiplexer Selection
       These bits select the input channel. When MUX3 is cleared, the MSB of the 4-channel multiplexer is cleared.
       *  0000 = ADC0
       *  0001 = ADC1 <--- we are using this channel
       *  0010 = ADC2
       *  0011 = ADC3
       *  0100 = ADC4
       *  0101 = ADC5
       *  0110 = ADC6
       *  0111 = ADC7
       *  1000 = ADC0
       *  1001 = ADC1
       *  1010 = ADC2
       *  1011 = ADC3
       *  1100 = ADC4
       *  1101 = ADC5
       *  1110 = ADC6
       *  1111 = ADC7
     */

    /*
     * A1 set as analog input
     * REF value is 5v
    */
    //ADMUX |= (1 << MUX0) | (1 << REFS0);
    
    /*
     * ADCSRA Register Description:
     * Bit 7 – ADIF: ADC Interrupt Flag
       This bit is set when the analog-to-digital conversion completes.
     * Bit 6 – ADIE: ADC Interrupt Enable
       When this bit is set, the ADC generates an interrupt request when the conversion completes.
     * Bit 5 – ADSC: ADC Start Conversion
       When this bit is set, the ADC starts the conversion.
     * Bit 4 – ADEN: ADC Enable
       When this bit is set, the ADC is enabled.
     * Bit 3 – ADPS2:0: ADC Prescaler Select
       These bits select the ADC clock prescaler. The prescaler can range from 2 to 128.
      *  000 = 2
      *  001 = 2
      *  010 = 4
      *  011 = 8 <------ we are using this
      *  100 = 16
      *  101 = 32
      *  110 = 64
      *  111 = 128
      * Bits 2 to 0 are not used.
    */

    /*
     * Enable ADC
     * Set ADC prescaler to 8 bit
    */
   ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1);
}

void oscilloscope(){

  /* Variables declaration */
  char adc_value_str[20];
  
  /* Calcolate ADC values */
  for (idx = 0; idx < 3; idx++){
   /* Set ADC channel */
    ADMUX |= (i == 0) ? (1 << MUX0) | (1 << REFS0) :
             (i == 1) ? (1 << MUX0) | (1 << MUX1) | (1<< REFS0) :
             (i == 2) ? (1 << MUX0) | (1 << MUX2) | (1<< REFS0);

    /* Start ADC conversion */
    ADCSRA |= (1 << ADSC);

    /* Wait for ADC conversion to complete */
    while(ADCSRA & (1 << ADSC));
    ADC_values[idx] = ADC * Volt_converter;
    ADMUX = 0;
  }
   
   sprintf(adc_value_str, "%.4f",interrupt_counter * sampling_time/1000);
   strcat(sent_message, adc_value_str);
   strcat(sent_message, "-");

   for (int i = 0; i < 3; i++) {
        sprintf(adc_value_str, "%.4f", ADC_values[0]);
        strcat(sent_message, adc_value_str);
        strcat(sent_message, i == 2 ? "\n" : " ");
   }

   UART_putString(sent_message);
}

int main(){

    /* Variables declaration */
    char user_input[1024];

    /* Inizialize UART */
    UART_init();

    /* Inizialize ADC */
    ADC_init();
    
    /* Get user input and convert it to float */
    UART_getString(user_input);

    sampling_time = atof(user_input);
    const int timer = sampling_time;

   /*
     * Configure timer
     * set prescaler to 1024
   */
   TCCR5A = 0;
   TCCR5B = (1 << WGM52) | (1 << CS50) | (1 << CS52);
   
   /*
     * Cloak is 16MHz
     * 1. Convert the desired interval to seconds: 1 ms = 0.001 s
     * 2. Calculate the number of clock cycles in this interval: 0.001 s * 16,000,000 Hz = 16,000 cycles
     * 3. The `OCR5A` register is 16 bits, so it can hold values up to 65,535. 
     *    The timer is in CTC mode (Clear Timer on Compare match), 
     *    the timer will reset to 0 every time it reaches the value in `OCR5A`. 
     *    So, to get a timer interval of 16,000 cycles, you would set `OCR5A` to 16,000.
     * 4. However, if you're using a prescaler (which is common in many microcontroller applications to divide the clock frequency 
     *    by a certain factor), you would need to adjust the value accordingly. For example, if you're using a prescaler of 1024, 
     *    you would calculate the `OCR5A` value like this: 16,000 / 1024 ≈ 15.625   
    */
   uint16_t ocr = (uint16_t)(15,625 * timer);
   OCR5A = ocr;

   cli();
    TIMSK5 |= (1 << OCIE5A);
   sei();

  
  while(1){
     while(!interrupt_occurred);
     interrupt_occurred = 0;
     
     printf("Interrupt Occurred\n");
  }
}