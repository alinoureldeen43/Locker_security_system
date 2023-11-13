/******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.c
 *
 * Description: Source file for the Timer driver
 *
 * Author: Ali Hassan
 *
 *******************************************************************************/
#include "timer.h"
#include "../../common_macros.h"
#include <avr/io.h>				/* to use TIMER1 registers */
#include <avr/interrupt.h>  	/* for TIMER1 ISR */



/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
static volatile void (*CallBack_ptr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_COMPA_vect)
{
	if(CallBack_ptr)
	{
		(*CallBack_ptr)();
	}
}

ISR(TIMER1_OVF_vect)
{
	if(CallBack_ptr)
	{
		(*CallBack_ptr)();
	}
}


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Initializes the Timer driver
 */
void Timer1_init(const Timer1_Config_t * Config_Ptr)
{
	/* 1. Check required timer mode*/
	switch (Config_Ptr->mode)
	{

	case TIMER1_NORMAL_MODE:
		/* if normal mode,
		 * 					load required initial value in TCNT1 register,
		 * 					Adjust WGM bits to normal mode
		 * 					enable overflow interrupt */
		TCNT1 = Config_Ptr->initial_value;

		TCCR1A = 0; /* noraml mode,  OC1A disconnected */

		SET_BIT(TIMSK, TOIE1); /* enable overflow interrupt */
		break;

	case TIMER1_CTC_MODE:
		/* if CTC mode,
		 * 				load required compare value in OCR1 register,
		 * 				Adjust WGM bits to CTC mode
		 * 				enable o/p compare match interrupt */
		OCR1A = Config_Ptr->compare_value;

		TCCR1A = 0; /* CTC mode,  OC1A disconnected */

		SET_BIT(TCCR1B, WGM12);	/* CTC mode: WGM12 bit = 1 */

		SET_BIT(TIMSK, OCIE1A); /* enable o/p compare match flag */
		break;
	}

	/* reset prescaler bits then assign the required prescaler value */
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);
}

/*
 * Description :
 * Disable the Timer driver
 */
void Timer1_deInit(void)
{
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	/* Clear timer1 used interrupt bits */
	CLEAR_BIT(TIMSK, TOIE1);
	CLEAR_BIT(TIMSK, OCIE1A);
}

/*
 * Description :
 * sets the Call Back function address
 */
void Timer1_setCallBack(void(*a_ptr)(void))
{
	if(a_ptr)
	{
		CallBack_ptr = a_ptr;
	}
	else
	{
		//a_ptr is null (error)
	}
}
