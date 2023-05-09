#include "trackerHw.h"

volatile uint32_t tick = 0;

void SysTick_Handler(void)
{
    tick++ ;
}

void HW_DelayMs(uint32_t _ms)
{
    static bool init = false;
    if( !init )
    {
        SysTick_Config( CPU_FREQUENCY / 1000 );
        init = true ;
    }

    uint32_t start = tick ;
    while( tick - start < _ms ) ;
}


void HW_trackerHwInit(void) {
	RCC->CR				= (RCC->CR & ~(1<<8)) | (1<<8); //Enable HSI 16MHz internal oscillator
	RCC->IOPENR 		|= RCC_IOPENR_GPIOAEN; //Enable GPIOA clock
	RCC->IOPENR 		|= RCC_IOPENR_GPIOBEN; //Enable GPIOB clock
	RCC->APBENR2		|= RCC_APBENR2_USART1EN; //Enable USART1 clock
	RCC->APBENR2		|= RCC_APBENR2_SPI1EN; //Enable SPI1 clock
	RCC->APBENR2		|= RCC_APBENR2_ADCEN; //Enable ADC clock
	RCC->APBENR1		|= RCC_APBENR1_TIM3EN; //Enable TIM3 clock

	GPIOA->MODER		= (GPIOA->MODER & ~(1<<0)) | (1<<0);
	GPIOA->MODER		= (GPIOA->MODER & ~(1<<1)) | (0<<1); //Setting LED pin as output

	//Setting the following bits breaks LORA
	//GPIOA->MODER		= (GPIOA->MODER & ~(1<<2)) | (0<<2);
	//GPIOA->MODER		= (GPIOA->MODER & ~(1<<3)) | (1<<3); //Pin 1 as input (DIO1)

	GPIOA->MODER		= (GPIOA->MODER & ~(1<<10)) | (0<<10);
	GPIOA->MODER		= (GPIOA->MODER & ~(1<<11)) | (1<<11); //Setting pin 5 as alternate function
	GPIOA->OSPEEDR		= (GPIOA->OSPEEDR & ~(1<<10)) | (1<<10);
	GPIOA->OSPEEDR		= (GPIOA->OSPEEDR & ~(1<<11)) | (1<<11); //Setting pin 5 to very high speed


	GPIOA->MODER		= (GPIOA->MODER & ~(1<<12)) | (0<<12);
	GPIOA->MODER		= (GPIOA->MODER & ~(1<<13)) | (1<<13); //Setting pin 6 as alternate function
	GPIOA->OSPEEDR		= (GPIOA->OSPEEDR & ~(1<<12)) | (1<<12);
	GPIOA->OSPEEDR		= (GPIOA->OSPEEDR & ~(1<<13)) | (1<<13); //Setting pin 6 to very high speed


	GPIOA->MODER		= (GPIOA->MODER & ~(1<<14)) | (0<<14);
	GPIOA->MODER		= (GPIOA->MODER & ~(1<<15)) | (1<<15); //Setting pin 7 as alternate function
	GPIOA->OSPEEDR		= (GPIOA->OSPEEDR & ~(1<<10)) | (1<<14);
	GPIOA->OSPEEDR		= (GPIOA->OSPEEDR & ~(1<<10)) | (1<<15); //Setting pin 7 to very high speed


	GPIOB->MODER		= (GPIOB->MODER & ~(1<<0)) | (1<<0);
	GPIOB->MODER		= (GPIOB->MODER & ~(1<<1)) | (0<<1); //Setting LORA CS pin as output

	GPIOB->MODER		= (GPIOB->MODER & ~(1<<12)) | (0<<12);
	GPIOB->MODER		= (GPIOB->MODER & ~(1<<13)) | (1<<13); //Set pin 6 as alternate function

	GPIOB->MODER		= (GPIOB->MODER & ~(1<<14)) | (0<<14);
	GPIOB->MODER		= (GPIOB->MODER & ~(1<<15)) | (1<<15); //Set pin 7 as alternate function

	HW_write_LORACS(1);
	SPI1->CR1			|= SPI_CR1_MSTR; //Set as master
	SPI1->CR1			|= SPI_CR1_BR_2;
	SPI1->CR1			|= SPI_CR1_SSM; 
	SPI1->CR1			|= SPI_CR1_SSI; //I dont really know what these do, but without them it doesnt work
	SPI1->CR2			|= SPI_CR2_FRXTH; //Make RXNE get set at 8bit fifo
	SPI1->CR1			|= SPI_CR1_SPE; //Enable SPI peripheral

	ADC1->CR			|= ADC_CR_ADVREGEN; //Enable ADC voltage regulator
	HW_DelayMs(2); //Give the regulator a moment to settle before we calibrate
	ADC1->CR			|= ADC_CR_ADCAL; //Start calibration
	while((ADC1->CR & ADC_CR_ADCAL) != 0); //Wait until ADCAL is reset by hardware, meaning calibration done
	ADC1->SMPR			|= (0b111); //Set SMP1 to max
	ADC->CCR			|= ADC_CCR_TSEN; //Temp sensor enable
	ADC1->ISR			|= ADC_ISR_ADRDY; //Set ADRDY bit to clear it
	ADC1->CR			|= ADC_CR_ADEN; //Send enable command
	while((ADC1->CR & ADC_ISR_ADRDY) != 1); //Wait until ADC is ready

	TIM3->PSC			= (CPU_FREQUENCY / 1000); //16000 prescaler, giving us a millisecond timer

	USART1->BRR			= (CPU_FREQUENCY / 9600); //Set baud
	USART1->CR1			|= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE_RXFNEIE); //Enable peripheral, receive, transmit, interrupt

	NVIC_SetPriorityGrouping(0);
	uint32_t uart_pri_encoding = NVIC_EncodePriority( 0, 1, 0 );
	NVIC_SetPriority(USART1_IRQn, uart_pri_encoding);
	NVIC_EnableIRQ(USART1_IRQn); //Start UART interrupt
}

void HW_writeLED(bool _value) {
	GPIOA->ODR			= (GPIOA->ODR & ~(1<<0)) | (_value<<0); //Write bit 0 of ODR
}

void HW_write_LORACS(bool _value) {
	GPIOB->ODR			= (GPIOB->ODR & ~(1<<0)) | (_value<<0); //Write bit 0 of ODR
}

void HW_send_UART(char* _txbuffer, uint8_t _size) {
	int count = _size;
	while(count--) {
		while(!(USART1->ISR & USART_ISR_TXE_TXFNF)) {};
		USART1->TDR = *_txbuffer++;
	}
}

void HW_SPI_WriteBuffer(uint8_t *data, uint32_t size) {
  __disable_irq();
  while(size--) {
	while(!(SPI1->SR & SPI_SR_TXE)) {};
	*((volatile uint8_t*) &(SPI1->DR)) = *data++;
	while((SPI1->SR & SPI_SR_BSY)) {};
  }
  uint8_t dummy;
  while(SPI1->SR & SPI_SR_RXNE) {
	dummy = *(volatile uint8_t *)&SPI1->DR;
  }
  __enable_irq();
}

void HW_SPI_ReadBuffer(uint8_t *data, uint32_t size) {
  __disable_irq();
  while(size--) {
	while(!(SPI1->SR & SPI_SR_TXE)) {};
	*((volatile uint8_t*) &(SPI1->DR)) = 0x00;
	while(!(SPI1->SR & SPI_SR_RXNE)) {};
	*data++ = *((volatile uint8_t*) &(SPI1->DR));
  }
  __enable_irq();
}

int16_t HW_readADC(uint8_t _ch) {
    int16_t result;

    while((ADC1->ISR & ADC_ISR_ADRDY) != 1); //Wait until ADC is ready
    ADC1->CHSELR = (1 << _ch); //Select channel
    ADC1->CR |= ADC_CR_ADSTART; //Start sampling
    while((ADC1->ISR & ADC_ISR_EOC) == 0); //Wait for conversion to complete
    result = ADC1->DR; //Read result from data register
    return result;
}

void HW_StartTimer3() {
	TIM3->CR1			|= TIM_CR1_CEN; //Enable timer
}

void HW_StopTimer3() {
	TIM3->CR1 &= ~TIM_CR1_CEN; // Disable timer
}

uint32_t HW_getTimer3() {
	return TIM3->CNT;
}

void HW_resetTimer3() {
	TIM3->CNT = 0;
}
