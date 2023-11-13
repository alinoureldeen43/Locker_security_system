/******************************************************************************
 *
 * Module: DC_MOTOR
 *
 * File Name: dc_motor.h
 *
 * Description: Header file for the DC_MOTOR driver
 *
 * Author: Ali Hassan
 *
 *******************************************************************************/

#ifndef HAL_DC_MOTOR_DC_MOTOR_H_
#define HAL_DC_MOTOR_DC_MOTOR_H_

#include "../../std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_MOTOR_PIN1_PORT_ID		PORTC_ID
#define DC_MOTOR_PIN1_PIN_ID		PIN2_ID


#define DC_MOTOR_PIN2_PORT_ID		PORTC_ID
#define DC_MOTOR_PIN2_PIN_ID		PIN3_ID

#define DC_MOTOR_EN_PORT_ID		PORTC_ID
#define DC_MOTOR_EN_PIN_ID		PIN4_ID



/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	CW,
	A_CW,
	STOP
}DcMotor_State;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void);

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on
 * the state input state value, Motor speed is always at max.
 *
 */
void DcMotor_Rotate(DcMotor_State state);
#endif /* HAL_DC_MOTOR_DC_MOTOR_H_ */
