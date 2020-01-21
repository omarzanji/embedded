#include "STM32L1xx.h"

static int counter; // Initialize count variable

void PinSetup () {
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	GPIOA->MODER &= 0xFFFFFFC3; // General purpose input mode for PA1 and PA2
	GPIOC->MODER &= 0xFFFFFF55; // output mode for PC0-3
}

void delay () {
	int stop = 16000000;
	for (int i=0; i<stop; i++) {         // outer loop
	}
}

void count (int direction) {
	int sw1 = GPIOA->IDR &= 0x0002; // Read PA1 (on or off)
	if (sw1) {
		if (direction == 0) { // forward
			counter++;
			if (counter == 10) {
				counter = 0;
			}
		} else {             // reverse
			counter--;
			if (counter == 0) {
				counter = 10;
			}
		}
	GPIOC->ODR &= counter;
	}
}

int main(void) {
	int direction; // direction
	PinSetup();
	direction = GPIOA->IDR &= 0x0004; // Read PA2 (direction 0-for 1-rev)
	if (direction) {
		counter = 10;
	} else {
		counter = 0;
	}
	while (1) {  // main loop
		direction = GPIOA->IDR &= 0x0004; // Read PA2 (direction 0-for 1-rev)
		count(direction);
		delay();
	}
}
