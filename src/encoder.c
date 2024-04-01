
#include "encoder.h"

//----------------------------------------------------------------------------------------
void encoder_init(void)
{
	#if ENC_TIMER == 1
		#define ENC_TIM TIM1
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	#endif

	#if ENC_TIMER == 2
		#define ENC_TIM TIM2
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	#endif

	#if ENC_TIMER == 3
		#define ENC_TIM TIM3
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	#endif

	#if ENC_TIMER == 4
		#define ENC_TIM TIM4
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	#endif


	ENC_TIM->CCMR1 = TIM_CCMR1_IC1F | TIM_CCMR1_IC2F; // | TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
	ENC_TIM->CCER  = TIM_CCER_CC1P | TIM_CCER_CC2P; //&= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
	ENC_TIM->SMCR  = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;; //TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
	ENC_TIM->ARR   = 0xffff;
	ENC_TIM->CNT   = 0;
	ENC_TIM->CR1   = TIM_CR1_CEN ;
}
//----------------------------------------------------------------------------------------
short encoder_get(void)
{
	return ENC_TIM->CNT;
}
//----------------------------------------------------------------------------------------
