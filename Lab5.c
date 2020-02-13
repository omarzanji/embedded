/*
Lab 5
Team Squid: Omar Barazanji and Peyton Spencer
2/12/19
*/

#include "STM32L1xx.h"

static int row, row_out, col, col_out, counter;
static int interrupt, button_num, counter_clk;

void PinSetup () {
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	RCC->AHBENR |= 0x02; // Enable GPIOB clock (bit 0)
	RCC->AHBENR |= 0x04; // Enable GPIOC clock (bit 3)

	GPIOA->MODER &= (~0x0);
	GPIOA->MODER |= 0x3; // input mode for PA1.
	GPIOB->MODER &= (0x0000);
	GPIOB->MODER |= 0x5500; // in mode PB3-0 and out mode PB7-4
	GPIOB->PUPDR = 0x55; //enable pullup resistors for PB3-0
	GPIOC->MODER |= 0x55;  // out mode PC3-0

	SYSCFG->EXTICR[1] &= 0x0; // Turns on EXTI1 for PA1
	EXTI->FTSR |= 0x0002;  // Sets EXTI1 to rising edge triggered
	EXTI->PR |= 0x0002;  // clears pending status of EXTI1
	EXTI->IMR |= 0x0002;  // disable mask for EXTI1
	NVIC_EnableIRQ(EXTI1_IRQn);
	__enable_irq();
}

void EXTI1_IRQHandler() {  // increasing button (1)
	__disable_irq();  // CPU disable IRQ
	interrupt = 0;
	for (col=0; col < 4; col++) {
		GPIOB->ODR |= 0xF0;
		GPIOB->ODR &= ~(1<<(col+4)); //engage column
		for (int i=0; i<50; i++) {  // short delay
		}
		for (row=0; row<4; row++) {
			counter_clk++;
			row_out = GPIOB->IDR; //read the IDR
			row_out &= 0x0F; //mask upper GPIOB pins
			row_out = row_out >> row; //shift the IDR bit to LEARN what row we're on!

			if ((row_out & 0x1) == 0) {
				row_out= row;
				col_out = col;
				interrupt = 1;  // raise successful interrupt flag
				break; //we all need a break
			}
		}
		if (interrupt) {
			break;
		}
	}
		for (int i=0; i<25600; i++) {  //  delay
	}
	if (interrupt)	{  // only output if successful
		GPIOC->ODR &= 0x0;
		button_num = ((row_out*4)+col_out);

		GPIOC->ODR |= button_num;
	}
	GPIOB->ODR &= 0x00;  // Drives output PB7-4 (col) low.
	for (int i=0; i<1600; i++) {  // debounce
	}
	EXTI->PR |= 0x0002;  // clears pending status of EXTI1
	NVIC_ClearPendingIRQ(EXTI1_IRQn);  // clear NVIC pending
	NVIC_EnableIRQ(EXTI1_IRQn);  // NVIC enable IRQ
	__enable_irq();  // CPU disable IRQ
}

void count (int display) {
		if (counter == 9) {
			counter = 0;
		} else {
			counter++;
		}
		if (display) {
			GPIOC->ODR &= 0x0;
			GPIOC->ODR |= counter; // output 4-bit counter
		}
}

int main(void) {
	PinSetup();
	col = 0;
	row = 0;
	counter = 0;
	interrupt = 0;
	GPIOB->ODR &= 0x0F;  // Drives column lines low.
	while (1) {  // main loop
		counter_clk++;
		if (counter_clk >= 2e4) {
			count(!interrupt);  // count when 2e4 interations (display count).
			counter_clk = 0;
		}
		if (interrupt) {
			counter_clk = 0;
			for (int i = 0; i < 1e2; i++) {
				for (int j = 0; j < 1e3; j++) {
				}
					counter_clk++;
					if (counter_clk >= 5e4) {
						count(!interrupt);  // count when 5e4 interations (no display)
						counter_clk = 0;
					}
			}
			interrupt = 0;
		}
	}
}
