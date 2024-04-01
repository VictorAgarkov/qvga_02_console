#ifndef GPIO_MODE_H_INCLUDED
#define GPIO_MODE_H_INCLUDED


	#include "stm32f10x_conf.h"



enum
{
	                       // 0 - input
  GPIO_Speed__10MHz = 1, // 1 - 10 MHz
  GPIO_Speed__2MHz  = 2, // 2 -  2 MHz
  GPIO_Speed__50MHz = 3  // 3 - 50 MHz
};


enum
{
	// input modes
	GPIO_Mode__IN_ANALOG = 0x0,
  GPIO_Mode__IN_FLOAT = 0x04,
  GPIO_Mode__IN_PULL = 0x08,

	// output modes
  GPIO_Mode__OUT_PP = 0x0,
  GPIO_Mode__OUT_OD = 0x4,
  GPIO_Mode__AF_PP = 0x8,
  GPIO_Mode__AF_OD = 0xC
};






	#define GPIO__CRVAL(gpio_cr, pin, val) gpio_cr = (gpio_cr & ~((uint32_t)0x0f << ((pin) * 4)))	 | (((val) & 0x0f) << ((pin) * 4))
	//#define GPIO_SET_MODE(gpio, pin, val) if((pin) < 8) {GPIO__CRVAL(gpio->CRL, (pin), (val));} else {GPIO__CRVAL(gpio->CRH, (pin) - 8, (val));}
	void gpio_set_mode(GPIO_TypeDef *gpio, int pin, int val);

#endif /* GPIO_MODE_H_INCLUDED */
