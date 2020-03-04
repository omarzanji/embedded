/*
Lab 8
Team Squid: Omar Barazanji and Peyton Spencer
3/3/19
*/

#include "STM32L1xx.h"

static int row, row_out, col, col_out;
static int good, button_num;
static int interrupts = 0;
static int freq = 10000;  // Hz
static int ARR_VAL;

void PinSetup () {
	ARR_VAL = (2097000 / freq); // ARRRRRRRRRRRRRRRRRG
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	RCC->AHBENR |= 0x02; // Enable GPIOB clock (bit 0)
	RCC->AHBENR |= 0x04; // Enable GPIOC clock (bit 3)
	//TIM 10 CONFIG
	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;  // enable clock to timer TIM10
	TIM10->DIER &= 0;
	TIM10->DIER |= 0x2;  // TIM10 CC1IE interrput enable
	TIM10->ARR = ARR_VAL; //Count period = 2.097 kHz
	TIM10->PSC = 0x0;  // Prescale = 0 (no prescale)
	TIM10->CCMR1 &= 0;
	TIM10->CCMR1 |= 0x60; //PWM mode 1, output CCR mode
	TIM10->CCER &= 0;
	TIM10->CCER |= 0x01; //enable CC1 and outputcompare is active high

	GPIOA->MODER &= ~(0x0);
	GPIOA->MODER |= 0x2003; // input mode for PA0.
	GPIOA->AFR[0] &= ~0x0F000000;
	GPIOA->AFR[0] |= 0x03000000; //AF3 for PA6- linking tim10

	GPIOB->MODER &= (0x0000);
	GPIOB->MODER |= 0x5500; // in mode PB3-0 and out mode PB7-4
	GPIOB->PUPDR = 0x55; //enable pullup resistors for PB3-0

	GPIOC->MODER &= 0x0;
	GPIOC->MODER |= 0x55;  // out mode for duty cycle value PC4-0

	SYSCFG->EXTICR[1] &= 0x0; // Turns on EXTI1 for PA1
	EXTI->FTSR |= 0x0002;  // Sets EXTI1 to falling edge triggered
	EXTI->PR |= 0x0002;  // clears pending status of EXTI1
	EXTI->IMR |= 0x0002;  // disable mask for EXTI1
	NVIC_EnableIRQ(EXTI1_IRQn); // NVIC enable IRQ
	NVIC_EnableIRQ(TIM10_IRQn); // NVIC enable IRQ
	__enable_irq();
	TIM10->CR1 |= TIM_CR1_CEN; //enable counter to count
	TIM10->CCR1 = 1*ARR_VAL/10; // initialize duty cycle (default 10%)
}


void EXTI1_IRQHandler() {  // increasing button (1)
	__disable_irq();  // CPU disable IRQ
	interrupts++;
	good = 0;
	for (col=0; col < 4; col++) {
		GPIOB->ODR |= 0xF0;
		GPIOB->ODR &= ~(1<<(col+4)); //engage column
		for (int i=0; i<50; i++) {  // short delay
		}
		for (row=0; row<4; row++) {
			row_out = GPIOB->IDR; //read the IDR
			row_out &= 0x0F; //mask upper GPIOB pins
			row_out = row_out >> row; //shift the IDR bit to LEARN what row we're on!

			if ((row_out & 0x1) == 0) {
				row_out= row;
				col_out = col;
				good = 1;  // raise successful interrput flag
				break; //we all need a break
			}
		}
		if (good) {
			break;
		}
	}
		for (int i=0; i<25600; i++) {  //  delay
			for (int j=0; j<1; j++) {
			}
		}
	if (good)	{  // only output if successful
		button_num = ((row_out*4)+col_out);
				TIM10->CCR1 = button_num*ARR_VAL/10; //
				GPIOC->ODR = button_num; //output the button numbro
			}

	GPIOB->ODR &= 0x00;  // Drives output PB7-4 (col) low.
	for (int i=0; i<1600; i++) {  // debounce
	}

	EXTI->PR |= 0x0002;  // clears pending status of EXTI1
	NVIC_ClearPendingIRQ(EXTI1_IRQn);  // clear NVIC pending
	NVIC_EnableIRQ(EXTI1_IRQn);  // NVIC enable IRQ
	__enable_irq();  // CPU disable IRQ
}


void TIM10_IRQHandler () { //handler for TIM10
	__disable_irq();  // CPU disable IRQ
	//TIM10->DIER &= ~(0x1);  // TIM10 update interrupt DISBALE
	TIM10->SR &= ~0x02;  // clears update interrupt flag
	NVIC_EnableIRQ(TIM10_IRQn); // NVIC enable
	NVIC_ClearPendingIRQ(TIM10_IRQn); // NVIC clear pending
	__enable_irq(); //re enable cpu
}


int main(void) {
	PinSetup();
	col = 0;
	row = 0;
	good = 0;
	GPIOB->ODR &= 0x0F;  // Drives column lines low.
	while (1) {  // main loop
		if (interrupts > 0) {
			__disable_irq();
			for (int i = 0; i < 4000; i++) {
			}
			interrupts = 0;
			__enable_irq();
		}
	}
}
