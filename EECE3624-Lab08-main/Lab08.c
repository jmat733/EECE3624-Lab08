/*****************************************************************************
 *	    File: Lab08.c
 *  Lab Name: RS232 Communication Server
 *    Author: Dr. Greg Nordstrom
 *   Created: 12/30/17
 *  Mod Date: 3/28/19, 1/17/21
 *  Modified: Jake Matthews
 *   ModDate: 10/20/2026
 * 
 * Processor: ATmega128A (on the ReadyAVR board)
 *
 * This program implements a communication server. Normally the server
 * simply echoes what was sent. However, there are two cases where
 * different behavior occurs:
 *    1. Pressing the ESC causes a soft reset (redisplays the title text)
 *    2. Typing the letters 'end' contiguously displays 'Bye' and halts
 *
 *****************************************************************************/

// define CPU speed (7.3728 MHz for the ATmega128A)
#define F_CPU   7372800UL

#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

#include <avr/io.h>
#include <util/delay.h>
#include "UARTLibrary.h"  // Double quotes for local library.

#define BAUDRATE 9600 // standard values from 1200 to 115200
#define DATABITS    8 // 5-8
#define STOPBITS    1 // 1,2
#define PARITY      0 // 0=none, 1=odd, 2=even

void printString(const char *str) {
	int i = 0;
	while (str[i] != '\0') {
		uart_tx(str[i]);
		i++;
	}
}

// Function to display the startup  message
void displayStartupMessage(void) {
	printString("\r\n\r\n");
	printString("RS232 Communications Server\r\n");
	printString("Status: Active\r\n");
	printString("Jake Matthews\r\n");
	printString("EECE3624\r\n");
	printString("Fall 2026\r\n");
	printString("\r\n\r\n");
}

int main(void)
{

    /* See the UARTLibrary.h for available calls.
	 *     	  // initialize the UART
	 *     	  uart_init(BAUDRATE, DATABITS, STOPBITS, PARITY);
	 *
	 *        // transmit a value (0x41) to the comm port
	 *        c = 0x41;
	 *        uart_tx(c);
	 *
	 *        // receive a byte from the comm port
	 *        c = uart_rx();
	 */

	unsigned char c;
	unsigned char endCount = 0;
	int halted = 0;

	
	
	DDRA |= (1 << PA0);
	PORTA |= (1 << PA0); // Turn LED off initially (if active low)

	uart_init(BAUDRATE, DATABITS, STOPBITS, PARITY);
	displayStartupMessage();
	
	while(1)
	{
		if (halted) {
			// Stop responding to input after "end" sequence until hard reset
			continue;
		}

		// Receive character from PuTTY
		c = uart_rx();

		// character received by blinking LED0 for 75ms
		PORTA &= ~(1 << PA0); // Turn LED0 ON 
		_delay_ms(75);
		PORTA |= (1 << PA0);  // Turn LED0 OFF

		// Check for "Esc" key 
		if (c == ESC) {
			displayStartupMessage();
			endCount = 0;
			continue;
		}

		// Check for 'end' 
		if (endCount == 0) {
			if (c == 'e') endCount = 1;
			else endCount = 0;
			} else if (endCount == 1) {
			if (c == 'n') endCount = 2;
			else if (c == 'e') endCount = 1;
			else endCount = 0;
			} else if (endCount == 2) {
			if (c == 'd') {
				endCount = 3;
				} else if (c == 'e') {
				endCount = 1;
				} else {
				endCount = 0;
			}
		}

		// Echo character back
		if (c == CR) {
			uart_tx(CR);
			uart_tx(LF); // Send LF automatically
			} else {
			uart_tx(c);
		}

		// If "end" sequence is fully matched
		if (endCount == 3) {
			printString("\r\n\r\nBye\r\n");
			halted = 1; // Stop responding
		}
		
	}
	return 0;
}
