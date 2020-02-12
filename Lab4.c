/*
Lab 4
Team Squid: Omar Barazanji and Peyton Spencer
2/5/19
*/

#include "STM32L1xx.h"

static int counter1; // Initialize counter1 variable (main)
static int counter2; // Initialize counter2 variable (inverse)
static int hex = 0;
static int count2_flag;
static int stop;
unsigned char count2_dir; // direction


void PinSetup () {
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	RCC->AHBENR |= 0x04; // Enable GPIOC clock (bit 3)
	GPIOA->MODER &= 0xFFFFFFF0; // input mode for PA1 and PA0 (sw1 and USER btn)
	GPIOC->MODER &= ~0xF;
	GPIOC->MODER |= 0x00055555; // output mode for PC9-0 (main counter)

	SYSCFG->EXTICR[0] &= 0xFF00; // Turns on EXTI1 and EXTI0 for PA1 and PA0
	EXTI->RTSR |= 0x0003;  // Sets EXTI1 and EXTI0 to rising edge triggered
	EXTI->PR |= 0x0003;  // clears pending status of EXTI0 and 1
	EXTI->IMR |= 0x0003;  // flags
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	__enable_irq();
}

void EXTI0_IRQHandler() {  // decreasing button (0)
	__disable_irq();  // CPU disable IRQ
	count2_dir = 0;
	EXTI->PR |= 0x0003;  // clears pending status of EXTI0 and 1
	GPIOC->ODR ^= 0x0100;  // turns on decreasing LED... PC8 - blue.
	NVIC_ClearPendingIRQ(EXTI0_IRQn);  // clear NVIC pending
	NVIC_EnableIRQ(EXTI0_IRQn);
	__enable_irq(); // CPU enable IRQ
}

void EXTI1_IRQHandler() {  // increasing button (1)
	__disable_irq();  // CPU disable IRQ
	count2_dir = 1;
	EXTI->PR |= 0x0003;  // clears pending status of EXTI0 and 1
	GPIOC->ODR ^= 0x0200;  // turns on increasing LED... PC9 - green.
	NVIC_ClearPendingIRQ(EXTI1_IRQn);  // clear NVIC pending
	NVIC_EnableIRQ(EXTI1_IRQn);  // NVIC enable IRQ
	__enable_irq();  // CPU disable IRQ
}

void delay (int stop) {
	for (int i=0; i<stop; i++) {
	}
}

void count (unsigned char count2_dir, unsigned char count2_flag) {
		//counter1++;
		if (counter1 == 9) {
			counter1 = 0;
		} else {
			counter1++;
		}
		if (count2_dir == 1) { // main in forward count
			if (counter2 == 9) {
				counter2 = 0;
			}
			if (count2_flag == 1) {
				counter2++;
			}
		}
		if (count2_dir == 0) { // main in reverse count
			if (counter2 == 0) {
				counter2 = 9;
			}
			if (count2_flag == 1) {
				counter2--;
			}
		}

	hex = 0;
	hex |= counter1;
	hex = hex << 4;
	hex |= counter2;
	GPIOC->ODR ^= hex; // output both counters
}

int main(void) {
	PinSetup();
	counter1 = 0;
	counter2 = 0;
	count2_dir = 1;
	count2_flag = 0;
	stop = 200000;
	while (1) {  // main loop
		count(count2_dir, count2_flag);
		count2_flag ^= 1;
		delay(stop);
	}
}
