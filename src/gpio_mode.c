
#include "gpio_mode.h"

void gpio_set_mode(GPIO_TypeDef *gpio, int pin, int val)
{
	if((pin) < 8)
	{
		GPIO__CRVAL(gpio->CRL, (pin), (val));
	}
	else
	{
		GPIO__CRVAL(gpio->CRH, (pin) - 8, (val));
	}
}

