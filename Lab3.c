/*
Lab 3
Team Squid: Omar Barazanji and Peyton Spencer
1/27/19
*/

#include "STM32L1xx.h"

static int counter1; // Initialize counter1 variable (main)
static int counter2; // Initialize counter2 variable (inverse)
static int hex = 0;

unsigned char sw2; // direction


void PinSetup () {
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	RCC->AHBENR |= 0x04; // Enable GPIOC clock (bit 3)

	GPIOA->MODER &= 0xFFFFFFC3; // input mode for PA1 and PA2 (sw1 and sw2)

	GPIOC->MODER &= ~0xF;
	GPIOC->MODER |= 0x00005555; // output mode for PC0-3 (main counter)
}

void delay () {
	int stop = 200000;
	for (int i=0; i<stop; i++) {         // outer loop
	}
}

void count (unsigned char sw2) {
	unsigned char sw1 = GPIOA->IDR &= 0x0002; // Read PA1 (on or off)
	sw1 = sw1 >> 1; // shift to fix bit offset
	if (sw1 == 1) {
		if (sw2 == 0) { // main in forward count
			counter1++;
			counter2--;
			if (counter1 == 9) {
				counter1 = 0;
				counter2 = 9;
			}
		} else {       // main in reverse count
			counter1--;
			counter2++;
			if (counter1 == 0) {
				counter1 = 9;
				counter2 = 0;
			}
		}
	}

	hex = 0;
	hex |= counter1;
	hex = hex << 4;
	hex |= counter2;

  GPIOC->ODR &= 0x00;
	GPIOC->ODR |= hex; // output main counter

}

int main(void) {
	PinSetup();
	sw2 = GPIOA->IDR &= 0x0004; // Read PA2 (direction)
	sw2 = sw2 >> 2; // shift to fix bit offset
	if (sw2 == 1) {
		counter1 = 9;
		counter2 = 0;
	} else {
		counter1 = 0;
		counter2 = 9;
	}
	while (1) {  // main loop
		sw2 = GPIOA->IDR &= 0x0004; // Read PA2 (direction)
		sw2 = sw2 >> 2;
		count(sw2);
		delay();
	}
}
