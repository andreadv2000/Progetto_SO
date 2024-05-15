
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "./avr_common/uart.h"

//******************Setting Output Pins******************//

#define TCCR1A_MASK (1<<COM1B1)|(1<<COM1B0)|(1<<WGM10) // PIN 12
#define TCCR1B_MASK (1<<CS10)|(1<<CS11)|(1<<WGM12) // PIN 12
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
char sent_message[256];

//******************Interrupt Service Routine******************//

ISR(TIMER5_COMPA_vect){
    interrupt_occurred = 1;
    interrupt_counter++;
}
//********************************************************/

void ADC_init(void){
    cli();
    /* Clear the ADMUX register */
    ADMUX = 0;

    /* Clear the ADCSRA register */
    ADCSRA = 0;
    /*
     * Enable ADC
     * Set ADC prescaler to 32 => (16000000/32 = 500000) => 500KHz that means 2us for each conversion
    */
   ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS2);
   sei();
}

void oscilloscope(void){

  /* Variables declaration */
  float Volt_converter = 5/1023.0;
  char adc_value_str[20];
  float ADC_values[3] = {0.0,0.0,0.0};

    /*
     * A1, A2, A3 set as analog input
     * REF value is 5v
    */

  uint8_t MUX_settings[] = {
    (1 << MUX0) | (1 << REFS0),
    (1 << MUX0) | (1 << MUX1) | (1<< REFS0),
    (1 << MUX2) | (1 << MUX0) | (1<< REFS0)
  };

  

  /* Calcolate ADC values */
  for (idx = 0; idx < 3; idx++){

    /* Set ADC channel */
    ADMUX |= MUX_settings[idx];

    float ADC_sum = 0.0;

    /* 
     * Take sub-samples and average them 
     * for the rumore reduction
    */
    for(int k = 0; k < 200; k++){
       /* Start ADC conversion */
       ADCSRA |= (1 << ADSC);

       /* Wait for ADC conversion to complete */
       while(ADCSRA & (1 << ADSC));

       ADC_sum += (float)ADC;
    }

    /* Calculate average */
    ADC_values[idx] = (ADC_sum / 200.0) * Volt_converter;

    /* Reset ADMUX */
    ADMUX = 0;
  }
   
   dtostrf(interrupt_counter * sampling_time/1000, 4, 4, adc_value_str);
   strcat(sent_message, adc_value_str);
   strcat(sent_message, "-");

   for (int i = 0; i < 3; i++) {
        dtostrf(ADC_values[i], 4, 4, adc_value_str);
        strcat(sent_message, adc_value_str);
        strcat(sent_message, i == 2 ? "\n" : "-");
   }

   UART_putString(sent_message);
   memset(sent_message, 0, sizeof(sent_message));
   memset(ADC_values, 0, sizeof(ADC_values));
}  

int main(void){

    /* Variables declaration */
    char user_input[256];

    /* Port Settings */
    const uint8_t portb_mask = (1<<6); // PIN 12
    const uint8_t porth_mask = (1<<3); // PIN 6
    const uint8_t porte_mask = (1<<4); // PIN 2

    /* Inizialize UART */
    UART_init();
    sei();
    
    /* Inizialize ADC */
    ADC_init();
    
    /* Get user input and convert it to float */
    
    UART_getString(user_input);
    
    sampling_time = atof(user_input);
    float timer = sampling_time;
    
    /* Timer and Wave Generator Settings */
    TCCR1A = TCCR1A_MASK; //PIN 12
    TCCR1B = TCCR1B_MASK; //PIN 12
    TCCR4A = TCCR4A_MASK; //PIN 6
    TCCR4B = TCCR4B_MASK; //PIN 6
    TCCR3A = TCCR3A_MASK; //PIN 2
    TCCR3B = TCCR3B_MASK; //PIN 2

    /*
     * Configure timer
     * set prescaler to 1024
    */
    TCCR5A = 0;
    TCCR5B = (1 << WGM52) | (1 << CS50) | (1 << CS52);
   
   
    /* Set the output compare register for timer 5 */
    uint16_t ocr = (uint16_t)(15.625 * timer);
    OCR5A = ocr;
   

    /* Clear all bits of output compare for timer */
    OCR1BH = 0; //PIN 12
    OCR4AH = 0; //PIN 6
    OCR3BH = 0; //PIN 2
   
   
    /* Set the pin as output */
    DDRB |= portb_mask; //PIN 12
    DDRH |= porth_mask; //PIN 6
    DDRE |= porte_mask; //PIN 2

   /* Enable timer interrupt */
   cli();
    TIMSK5 |= (1 << OCIE5A);
   sei();
   
   while(1){
     while(!interrupt_occurred);
     interrupt_occurred = 0;
     
     /* Increment counters*/
     OCR1BL += 9; //PIN 12
     OCR4AL += 12; //PIN 6
     OCR3BL += 15; //PIN 2

     oscilloscope();
     

     if((interrupt_counter+1)*sampling_time > 60000) return 0;
  }
}