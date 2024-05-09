#pragma once

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#define BAUD 19600
#define MYUBRR (F_CPU/16/BAUD-1)


void UART_init(void);

void UART_putChar(char c);
char UART_getChar(void);

char UART_getString(char* buf);
void UART_putString(char* buf);


