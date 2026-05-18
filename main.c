#include <stdint.h>

// RCC (Reset and Clock Control)
#define RCC_APB1ENR (*(volatile uint32_t*)0x4002101C)
#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)
#define RCC_CFGR    (*(volatile uint32_t*)0x40021004)

// GPIOA (General Purpose Input/Output Port A)
#define GPIOA_CRL (*(volatile uint32_t*)0x40010800)
#define GPIOA_IDR (*(volatile uint32_t*)0x40010808)
#define GPIOA_ODR (*(volatile uint32_t*)0x4001080C)

// ADC1 (Analog to Digital Converter)
#define ADC_SR          (*(volatile uint32_t*)0x40012400)
#define ADC_CR2         (*(volatile uint32_t*)0x40012408)
#define ADC_SMPR2       (*(volatile uint32_t*)0x40012410)
#define ADC_SQR3        (*(volatile uint32_t*)0x40012434)
#define ADC_DR          (*(volatile uint32_t*)0x4001244C)

// EXT_INTERRUPT (External Interrupt/Event Controller)
#define EXTI_IMR        (*(volatile uint32_t*)0x40010400)
#define EXTI_RTSR       (*(volatile uint32_t*)0x40010408)
#define EXTI_FTSR       (*(volatile uint32_t*)0x4001040C)
#define EXTI_PR         (*(volatile uint32_t*)0x40010414)
#define NVIC_ISER0      (*(volatile uint32_t*)0xE000E100)

// AFIO (Alternate Function I/O)
#define AFIO_EXTICR1    (*(volatile uint32_t*)0x40010008)


void delay(volatile uint32_t count)
{
    while(count > 0)
    {
        count--;
    }
}


volatile uint8_t rain_detected = 0;
volatile uint16_t adc_val;


void EXTI2_IRQHandler(void)
{
	// Check if the EXTI line 2 pending bit is set
		if (EXTI_PR & (1 << 2))
		{
		    EXTI_PR |= (1 << 2);    // Clear the pending flag by writing 1 to the bit
		    rain_detected = 0;      // Set flag to signal rain detected (0 = RAIN, 1 = NO RAIN)

		    // 1. Turn pump OFF (PA3, bit 3, is driven LOW)
		    GPIOA_ODR|=(0x1<<3);      // Pump OFF

		    // 2. Blink LED (PA4, bit 4)
		    GPIOA_ODR |= (1 << 4);
		    for (int i = 0; i < 300000; i++);
		    GPIOA_ODR &= ~(1 << 4);
		    for (int i = 0; i < 300000; i++);
		}
}

void ADC1_Init(void)
{
	    RCC_APB2ENR |= (1 << 9); // ADC1 clock
	    RCC_APB2ENR |= (1 << 2); // GPIOA

	    GPIOA_CRL &= ~(0xF << 0); // PA0 analog

	    ADC_CR2 |= (1 << 0);     // ADON ON
	    for(volatile int i=0;i<1000;i++);

	    ADC_SMPR2 &= ~(0x7 << 0);   // clear
	    ADC_SMPR2 |=  (0x7 << 0);   // Sample Time: Max sample time for stability 239.5    ADC clock cycle = 1 / 12,000,000 sec ≈ 83 ns   Time = 239.5 × 83 ns ≈ 20 µs

	    ADC_CR2 |= (1 << 3);     // RSTCAL
	    while (ADC_CR2 & (1 << 3));

	    ADC_CR2 |= (1 << 2);     // CAL
	    while (ADC_SR & (1 << 2));

	    // EXTSEL + EXTTRIG for SWSTART
	    ADC_CR2 |= (7 << 17);
	    ADC_CR2 |= (1 << 20);
}

uint16_t ADC_Read(uint8_t channel)
{
	 ADC_SQR3 = channel;

	 ADC_CR2 |= (1 << 22);   // SWSTART

	    while (!(ADC_SR & (1 << 1)));

	    return ADC_DR;
}


int main(void)
{
	// Enable Clocks: ADC1 | GPIOA |AFIO

	   // RCC_APB2ENR |= (1<<2);
		RCC_APB2ENR |= (1<<0);
		ADC1_Init(); // Initiate ADC read

	    // GPIO Configuration: Clear and Set Modes for Pins PA0-PA4

	    // PA3 (Pump): Output
		GPIOA_CRL&=~(0XF<<12);
		GPIOA_CRL|=(0X1<<12);
		GPIOA_ODR|=(0x1<<4);

	    // PA4 (LED): Output
		GPIOA_CRL&=~(0XF<<16);
		GPIOA_CRL|=(0X1<<16);
		GPIOA_ODR|=(0x1<<4);

	    // PA1 (External Switch): Input Pull-up/Pull-down
		GPIOA_CRL&=~(0XF<<4);
		GPIOA_CRL|=(0X8<<4);
		GPIOA_ODR|=(0x1<<1); // Enable internal Pull-up on PA1



	    // PA2 (Rain Sensor): Input Pull-up/Pull-down
		GPIOA_CRL&=~(0XF<<8);
		GPIOA_CRL|=(0x8<<8);
		GPIOA_ODR|=(1<<2); // Enable internal Pull-up on PA2




		// EXTI Setup (Rain Sensor on PA2)

		// Route EXTI Line 2 to PA2
		AFIO_EXTICR1 &= ~(0xF << 8);

		// Unmask EXTI Line 2 interrupt
		EXTI_IMR |= (1 << 2);

		// Set EXTI Line 2 to trigger on the Falling Edge
		EXTI_FTSR |= (1 << 2);
		EXTI_RTSR &= ~(1 << 2); // Ensure Rising Edge is disabled

		// Enable EXTI2 interrupt in NVIC
		NVIC_ISER0 |= (1 << 8);                 //allow CPU to accept interrupt there are total 68 external interrupt

	    // Main Loop
		    while (1)
		    {
	        // Check PA1 (External Safety Switch) status
		    if ((GPIOA_IDR &(1<<1))==0) // If PA1 is LOW (Switch is ON/Active)
		    {

		        // If rain_detected is 0 (set by ISR), rain is detected.
		      if (rain_detected == 0)
		    	 {
		    	   delay(10000);
		    	   rain_detected = 1; // Clear rain flag
		    	 }
		      else // rain_detected == 1 (NO RAIN)
		    	 {



		    	 GPIOA_ODR |= (0x1<<4); // Keep LED ON during check
		    	 adc_val = ADC_Read(0);
		    		 // Check if soil is dry (ADC value > 1400)
		    		 if (adc_val>3450)
		    		 {
		    		   GPIOA_ODR|=(0x1<<4);         //LED ON
		    		   GPIOA_ODR&=~(0x1<<3);      // Pump ON (PA3)

		    		 }
		    		 else // Soil is wet
		    		 {
		    		   GPIOA_ODR|=(0x1<<3);      // Pump OFF
		    		   GPIOA_ODR &=~(0x1<<4);     // LED OFF
		    		 }

		        }
		    }
		    	else
		    	{

		    		GPIOA_ODR|=(0x1<<3);      // Pump OFF
		    	    GPIOA_ODR &=~(0x1<<4);     // LED OFF
		    	}
		    	adc_val=0;


		    }
}
