/*
Lab 6
Team Squid: Omar Barazanji and Peyton Spencer
2/17/19
*/

#include "STM32L1xx.h"

static int row, row_out, col, col_out, counter;
static int good, button_num;
static int count_upper, count_lower, count_output;
static int state = 1;
static int count = 0;

void PinSetup () {
	RCC->AHBENR |= 0x01; // Enable GPIOA clock (bit 0)
	RCC->AHBENR |= 0x02; // Enable GPIOB clock (bit 0)
	RCC->AHBENR |= 0x04; // Enable GPIOC clock (bit 3)

	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;  // enable clock to timer TIM10
	TIM10->DIER |= TIM_DIER_UIE;  // TIM10 update interrput enable
	TIM10->ARR = 0x2000;
	TIM10->PSC = 0x19;  // T-overflow = 0.1 sec

	GPIOA->MODER &= (~0x0);
	GPIOA->MODER |= 0x3; // input mode for PA1.
	GPIOB->MODER &= (0x0000);

	GPIOB->MODER |= 0x5500; // in mode PB3-0 and out mode PB7-4
	GPIOB->PUPDR = 0x55; //enable pullup resistors for PB3-0

	GPIOC->MODER &= 0x0;
	GPIOC->MODER |= 0x5555;  // out mode PC7-0

	SYSCFG->EXTICR[1] &= 0x0; // Turns on EXTI1 for PA1
	EXTI->FTSR |= 0x0002;  // Sets EXTI1 to rising edge triggered
	EXTI->PR |= 0x0002;  // clears pending status of EXTI1
	EXTI->IMR |= 0x0002;  // disable mask for EXTI1
	NVIC_EnableIRQ(EXTI1_IRQn); // NVIC enable IRQ
	NVIC_EnableIRQ(TIM10_IRQn); // NVIC enable IRQ
	__enable_irq();
	TIM10->CR1 |= TIM_CR1_CEN; //enable counter to count
}


void EXTI1_IRQHandler() {  // increasing button (1)
	__disable_irq();  // CPU disable IRQ
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
	}
	if (good)	{  // only output if successful
		button_num = ((row_out*4)+col_out);
			if (button_num == 0) {
				state ^= state;
				if (state == 0) {
					TIM10->CR1 &= ~TIM_CR1_CEN; //disable counter
				}
				if (state == 1) {
					TIM10->CR1 &= TIM_CR1_CEN; //enable counter
				}
			}
			if (button_num == 1 && ~state) {
				TIM10->CNT = 0;
			}
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

	count += 1;
	if (count>=99) {
		count = 0;
	}

	count_upper = count / 10;
	count_lower = count - count_upper*10;
	count_output = (count_upper << 4) + count_lower;
	GPIOC->ODR = count_output;

	TIM10->SR &= ~0x01;  // clears update interrupt flag
	NVIC_EnableIRQ(TIM10_IRQn); // NVIC enable
	NVIC_ClearPendingIRQ(TIM10_IRQn); // NVIC clear pending
	__enable_irq(); //re enable cpu
}


int main(void) {
	PinSetup();
	col = 0;
	row = 0;
	counter = 0;
	good = 0;
	GPIOB->ODR &= 0x0F;  // Drives column lines low.
	while (1) {  // main loop

	}
}
