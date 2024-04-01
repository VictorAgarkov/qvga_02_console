#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

	#include "stm32f10x_conf.h"

	#define ENC_TIMER 4

	void  encoder_init(void);
	short encoder_get(void);


#endif /* ENCODER_H_INCLUDED */
