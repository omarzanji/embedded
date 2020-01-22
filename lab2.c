#include "STM32L1xx.h"

static int counter; // Initialize count variable
unsigned char direction; // direction


void PinSetup () {
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	RCC->AHBENR |= 0x04;
	GPIOA->MODER &= 0xFFFFFFC3; // General purpose input mode for PA1 and PA2
	GPIOC->MODER &= ~0xF;
	GPIOC->MODER |= 0x00000055; // output mode for PC0-3
}

void delay () {
	int stop = 1250000;
	for (int i=0; i<stop; i++) {         // outer loop
	}
}

void count (unsigned char direction) {
	unsigned char sw1 = GPIOA->IDR &= 0x0002; // Read PA1 (on or off)
	sw1 = sw1 >> 1;
	if (sw1 == 1) {
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
	}
  GPIOC->ODR &= ~0xF;
	GPIOC->ODR |= counter;
}

int main(void) {
	PinSetup();
	direction = GPIOA->IDR &= 0x0004; // Read PA2 (direction 0-for 1-rev)
	direction = direction >> 2;
	if (direction == 1) {
		counter = 10;
	} else {
		counter = 0;
	}
	while (1) {  // main loop
		direction = GPIOA->IDR &= 0x0004; // Read PA2 (direction 0-for 1-rev)
		direction = direction >> 2;
		count(direction);
		delay();
	}
}
