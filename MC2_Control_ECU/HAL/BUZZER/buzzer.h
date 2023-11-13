 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 * Author: Ali Hassan
 *
 *******************************************************************************/


#ifndef HAL_BUZZER_BUZZER_H_
#define HAL_BUZZER_BUZZER_H_


#include "../../std_types.h"
#include "../../MCAL/GPIO/gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID			PORTC_ID
#define BUZZER_PIN_ID			PIN6_ID



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Setup the direction for the buzzer pin as output pin through the GPIO driver.
 */
void Buzzer_init(void);

/*
 * Description :
 * Function to enable the Buzzer through the GPIO.
 */
void Buzzer_on(void);

/*
 * Description :
 * Function to disable the Buzzer through the GPIO
 */
void Buzzer_off(void);

#endif /* HAL_BUZZER_BUZZER_H_ */
