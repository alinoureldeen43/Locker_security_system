/******************************************************************************
 *
 * Module: APP
 *
 * File Name: app.c
 *
 * Description: Source file for the application
 *
 * Author: Ali Hassan
 *
 *******************************************************************************/
#include "app.h"
#include <avr/io.h>
#include <avr/delay.h>
#include "../MCAL/UART/uart.h"
#include "../MCAL/TWI/twi.h"
#include "../HAL/BUZZER/buzzer.h"
#include "../HAL/DC_MOTOR/dc_motor.h"
#include "../HAL/EEPROM/external_eeprom.h"
#include "../MCAL/TIMER/timer.h"
#include "../HAL/BUZZER/buzzer.h"

#define EEPROM_PASSWORD_LOCATION 0X0311


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * 		This function is responsible for setting and updating the password of the system
 */
void setPassword(void);

/*
 * Description :
 * 		The function is to check if the passed two passwords are identical
 */
void verifyPassword(void);

/*
 * Description :
 * 		The function is to check if the passed two passwords are identical
 */
void openGate(void);

/*
 * Description :
 * 		The function is to check if the passed two passwords are identical
 */
void lockSystem(void);

/*
 * Description :
 * 		This function is to compare user entered password && system password
 * Return:
 * 			1: Passwords match
 * 			0: Passwords do not match
 *
 */
uint8 isPassMatched(uint8 * pass1, uint8 * pass2, uint8 size);

/*
 * Description :
 * 			The required function to be executed when timer interrupt is fired,
 * 			"increments ticks variable"
 */
void TIMER1_callback_function(void);

/*
 * Description :
 * 			This function is to generate 15 seconds delay using timer1.
 */
void TIMER1_delay_15sec(void);

/*
 * Description :
 * 			This function is to generate 3 seconds delay using timer1.
 */
void TIMER1_delay_3sec(void);




/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
/* used to indicate the password size, to know how many bytes to read form EEPROM*/
uint8 pass_size = 0;

/* used to store the required number of ticks of timer1 to generate a certain delay*/
volatile uint8 ticks = 0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * This function is responsible for initializing the peripherals used
 */
void APP_init(void)
{

	UART_Config_t config = {UART_8_DATA_BITS, UART_PARITY_DISABLED,
			UART_1_STOP_BIT, 9600};

	/* Enable Global Interrupt */
	SREG |= (1<<7);
	TWI_init();
	DcMotor_Init();
	UART_init(&config);
	Buzzer_init();
}

/*
 * Description :
 * This function is responsible for operating the system as requried
 */
void APP_start(void)
{
	/* used to identify the required operation sent by HMI_ECU */
	uint8 operation_id;

	operation_id = UART_recieveByte();

	switch(operation_id)
	{
	case '0':	/* Setting a new password operation */
		setPassword();
		break;

	case '1':	/* Check if user entered password is correct */
		verifyPassword();
		break;


	case'2':	/* open gate operation */
		openGate();
		break;


	case '3':	/* lock the system */
		lockSystem();
		break;
	}
}


/*
 * Description :
 * 		This function is responsible for setting and updating the password of the system
 */
void setPassword(void)
{
	uint8 received_pass[10] = "";
	uint8 i;
	/* reset pass_size , i */
	pass_size = 0;
	i = 0;

	/* store the user entered password */
	UART_receiveString(received_pass);

	/* store password in eeprom */
	while(received_pass[i])
	{
		EEPROM_writeByte(EEPROM_PASSWORD_LOCATION + i, received_pass[i]);
		_delay_ms(10);
		pass_size++;
		i++;
	}
}


/*
 * Description :
 * 		The function is to check if the passed two passwords are identical
 */
void verifyPassword(void)
{
	/* isMathed is a flag that is set when password is correct,
	 * i is a counter used when reading from EEPROM
	 */
	uint8 isMatched, i;
	uint8 received_pass[10] = "";


	uint8 stored_pass[10] = ""; /* to store the password extracted from EEPROM */


	/* store the user entered password */
	UART_receiveString(received_pass);

	/* extract saved password from EEPROM */
	for(i = 0; i < pass_size; i++)
	{
		EEPROM_readByte(EEPROM_PASSWORD_LOCATION + i, &stored_pass[i]);
		_delay_ms(10);
	}

	/* check if the user entered password && stored password are identical */
	isMatched = isPassMatched(received_pass, stored_pass, pass_size);
	if(isMatched)
	{
		/* if matched, send '1' */
		UART_sendByte('1');
	}
	else
	{
		/* if not matched, send '0' */
		UART_sendByte('0');
	}

}

/*
 * Description :
 * 		This function is to compare user entered password && system password
 * Return:
 * 			1: Passwords match
 * 			0: Passwords do not match
 *
 */
uint8 isPassMatched(uint8 * pass1, uint8 * pass2, uint8 size)
{
	uint8 i = 0, matched = 1;
	for(; i < size; ++i)
	{
		if(pass1[i] == pass2[i])
		{
			continue;
		}
		else
		{
			matched = 0;
			break;
		}
	}

	return matched;
}

/*
 * Description :
 * 			This function is to generate 15 seconds delay using timer1.
 */
void TIMER1_delay_15sec(void)
{
	/* Timer operates in CTC mode, required OCR value to generate 15 seconds
	 * at 1024 pre-scaler is 58594 for 2 times,
	 * for TCNT1 element is configuration can be any dummy number'1000'*/
	Timer1_Config_t config = { 1000 , 58594, TIMER1_PRESCALER_1024, TIMER1_CTC_MODE};
	Timer1_init(&config);

	Timer1_setCallBack(TIMER1_callback_function);
	while(ticks < 2);
	ticks = 0;
	Timer1_deInit();
}


/*
 * Description :
 * 			This function is to generate 3 seconds delay using timer1.
 */
void TIMER1_delay_3sec(void)
{
	/* Timer operates in CTC mode, required OCR value to generate 3 seconds
	 * at 1024 pre-scaler is 23450,
	 * for TCNT1 element is configuration can be any dummy number'1000'*/
	Timer1_Config_t config = { 1000 , 23500, TIMER1_PRESCALER_1024, TIMER1_CTC_MODE};
	Timer1_init(&config);

	/* set callback function for timer1 interrupt */
	Timer1_setCallBack(TIMER1_callback_function);

	/* wait for timer interrupt */
	while(!ticks);

	/* reset ticks variable for next future use */
	ticks = 0;
	Timer1_deInit();
}

/*
 * Description :
 * 			The required function to be executed when timer interrupt is fired,
 * 			"increments ticks variable"
 */
void TIMER1_callback_function(void)
{
	ticks++;
}


/*
 * Description :
 * 		The function is to check if the passed two passwords are identical
 */
void lockSystem(void)
{
	/* The control ECU is required to turn on the buzzer for 1 minute when system
	 * goes to the locked state
	 */
	uint8 timer_counter = 0;	/* used to repeat the 15sec delay function to get 1 minute */

	/* turn on buzzer for 1 minute */
	Buzzer_on();
	while(timer_counter++ < 4)
	{
		TIMER1_delay_15sec();
	}
	Buzzer_off();
}

/*
 * Description :
 * 		The function is to check if the passed two passwords are identical
 */
void openGate(void)
{
	/* open the door by rotating the DC motor CW for 15 seconds */
	DcMotor_Rotate(CW);
	TIMER1_delay_15sec();

	/* keep the door open for 3 seconds */
	DcMotor_Rotate(STOP);
	TIMER1_delay_3sec();

	/* lock the door by rotating the DC motor ACW for 15 seconds */
	DcMotor_Rotate(A_CW);
	TIMER1_delay_15sec();

	/* stop the motor */
	DcMotor_Rotate(STOP);
}
