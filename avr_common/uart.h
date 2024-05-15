#pragma once

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD-1)

/* Dichiarazione della funzione per inizializzare l'UART */ 
void UART_init(void);

/* Dichiarazione della funzione per inviare un carattere via UART */
void UART_putChar(char c);

/* Dichiarazione della funzione per ricevere un carattere via UART */
char UART_getChar(void);

/* Dichiarazione della funzione per ricevere una stringa via UART */
char UART_getString(char* buf);

/* Dichiarazione della funzione per inviare una stringa via UART */
void UART_putString(char* buf);


