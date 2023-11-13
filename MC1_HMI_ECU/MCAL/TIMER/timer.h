/******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.h
 *
 * Description: Header file for the Timer driver
 *
 * Author: Ali Hassan
 *
 *******************************************************************************/

#ifndef MCAL_TIMER_TIMER_H_
#define MCAL_TIMER_TIMER_H_

#include "../../std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* This enum will be used to specify the prescaler used with Timer1 */
typedef enum
{
	TIMER1_NO_CLOCK,
	TIMER1_PRESCALER_1,
	TIMER1_PRESCALER_8,
	TIMER1_PRESCALER_64,
	TIMER1_PRESCALER_256,
	TIMER1_PRESCALER_1024,
}Timer1_Prescaler;

/* This enum will be used to specify the running mode of Timer1 */
typedef enum
{
	TIMER1_NORMAL_MODE,
	TIMER1_CTC_MODE
}Timer1_Mode;

/* This struct holds the initialization elements of Timer1 */
typedef struct{
	uint16 initial_value;
	uint16 compare_value; // it will be used in compare mode only.
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;
} Timer1_Config_t;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Initializes the Timer driver
 */
void Timer1_init(const Timer1_Config_t * Config_Ptr);

/*
 * Description :
 * Disable the Timer driver
 */
void Timer1_deInit(void);

/*
 * Description :
 * sets the Call Back function address
 */
void Timer1_setCallBack(void(*a_ptr)(void));
#endif /* MCAL_TIMER_TIMER_H_ */
