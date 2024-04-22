#ifdef UART_H
#define UART_H

#include <avr/io.h>

#define F_CPU 16000000UL
#define BAUD 19600
#define MYUBRR (F_CPU/16/BAUD-1)

void USART_init(){
    /* Set baund rate */
    UBBR0H = (uint8_t)(MYUBR>>8);
    UBBR0L = (uint8_t)MYUBRR;
    
    /* 8-bit data */
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);  
  
    /* Enable RX and TX */
    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
}

void UART_putChar(uint8_t c){
  /* 
    * Wait for transmission completed, 
    * looping on status bit
  */
  while ( !(UCSR0A & (1<<UDRE0)) );
  /* Start transmission */
  UDR0 = c;
}
uint8_t UART_getChar(void){
  /*
    * Wait for incoming data, 
    * looping on status bit 
  */
  while ( !(UCSR0A & (1<<RXC0)) );
  
  /* Return the data */
  return UDR0;
    
}

uint8_t UART_getString(uint8_t* buf){
  uint8_t* b0=buf; //beginning of buffer
  while(1){
    uint8_t c=UART_getChar();
    *buf=c; ++buf;
    // reading a 0 terminates the string
    if (c==0) return buf-b0;
    // reading a \n  or a \r return results
    // in forcedly terminating the string
    if(c=='\n'||c=='\r'){
      *buf=0; ++buf; return buf-b0;
    }
  }
}
void UART_putString(uint8_t* buf){
  while(*buf){
    UART_putChar(*buf); ++buf;
  }
}


#endif // UART_H