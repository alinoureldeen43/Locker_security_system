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
#include "../HAL/LCD/lcd.h"
#include <avr/io.h>
#include <avr/delay.h>
#include "../MCAL/UART/uart.h"
#include "../HAL/KEYPAD/keypad.h"
#include "../MCAL/TIMER/timer.h"



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * 		This function is responsible for setting and updating the password of the system
 */
void setPass(void);

/*
 * Description :
 * 		This function is responsible for prompting the user for correct passwords,
 * 		within maximum 3 times.
 * Return:
 * 			1 Password is correct.
 * 		   	0 All trials are used without password being correct
 */
uint8 checkPassword_trials(void);

/*
 * Description :
 * 			This function is responsible for executing the steps required to open the door
 */
void openDoor(void);

/*
 * Description :
 * 			This function is responsible for locking the systems when all password trials are used
 */
void lockSystem(void);



/*
 * Description :
 * 		This function is responsible for storing the user entered password,
 * 		and printing '*' on LCD instead of each entered character
 */
void getPass(uint8 * passArr, uint8 * size);

/*
 * Description :
 * 		This function is to compare passwords entered by user when setting a new password
 * Return:
 * 			1: Passwords match
 * 			0: Passwords do not match
 *
 */
uint8 isPassMatched(uint8 * pass1, uint8 * pass2, uint8 size);


/*
 * Description :
 * 		This function is to compare password entered by user to the saved password of the system
 * Return:
 * 			'1' Password is correct.
 * 		   	'0' Password is false.
 */
char verifyPass_ControlECU(void);


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
 * 			This function is to generate 1 second delay using timer1.
 */
void TIMER1_delay_1sec(void);




/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/

volatile uint8 ticks = 0; /* used to indicate that required timer ticks are acquired */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * This function is responsible for initializing the peripherals used
 */
void APP_init(void)
{
	/* Crate a UART configuration variable with the required properties */
	UART_Config_t config = {UART_8_DATA_BITS, UART_PARITY_DISABLED,
			UART_1_STOP_BIT, 9600};

	/* Enable Global Interrupt */
	SREG |= (1<<7);

	/* initialize LCD, UART modules */
	LCD_init();
	UART_init(&config);

	/* set password at startup */
	setPass();

}

/*
 * Description :
 * This function is responsible for operating the system as requried
 */
void APP_start(void)
{
	uint8 isCorrect;		/* used to check if entered password matches system password */
	uint8 input = '\0'; 	/* used to get the user required action */


	/* Display main system options */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, " + : Open Door");
	LCD_displayStringRowColumn(1, 0, " - : Change Pass");

	/* get user required action, keep prompting till a valid input is entered */
	do
	{
		input = KEYPAD_getPressedKey();
	}while(input != '+' && input != '-');

	/* ask user for system password with 3 trials allowance */
	isCorrect = checkPassword_trials();
	if(!isCorrect)
	{
		/* all the 3 trials are used, lock the system */
		lockSystem();

		/* go to step 2 "main menu options"*/
		return;
	}

	/* Execute the required action */
	if('+' == input)
	{
		openDoor();
	}
	else
	{
		/* change the password */
		setPass();
	}
}

/*========================================================================================================
  ======================================================================================================*/

/*
 * Description :
 * 		This function is responsible for setting and updating the password of the system
 */
void setPass(void)
{
	uint8 pass1[10] = ""; /* to store the first password */
	uint8 pass2[10] = ""; /* to store the confirmation password */
	uint8 pass1_size = 0; /* indicates pass1 length */
	uint8 pass2_size = 0; /* indicates pass2 length */

	uint8 matched = 0;	  /* flag that is set when pass1 && pass2 are identical */

	do
	{
		/* prompt user for password */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Plz enter pass: ");
		LCD_moveCursor(1, 0);

		/* get the password for the first time */
		getPass(pass1, &pass1_size);

		/* prompt user to confirm the password */
		LCD_displayStringRowColumn(0, 0, "Plz re-enter the");
		LCD_displayStringRowColumn(1, 0, "same pass: ");

		/* get the password for the second time */
		getPass(pass2, &pass2_size);

		/* Check if the two passwords match*/
		if(pass1_size != pass2_size)
		{
			/* if the two passwords are of different sizes, they are already mismatched */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "Error!! ");
			LCD_displayStringRowColumn(1, 0, "NOT MATCHED");
			_delay_ms(1000);
			continue;
		}
		else
		{
			/* if the 2 passwords are the same size, check if they match */
			matched = isPassMatched(pass1, pass2, pass1_size);

			LCD_clearScreen();

			if(matched)
			{
				/* if matched, send the password to the Control_ECU to be stored in EEPROM */
				LCD_displayStringRowColumn(0, 0, "Pass set");
				LCD_displayStringRowColumn(1, 0, "Successfully");

				UART_sendByte('0');
				UART_sendString(pass1);

			}
			else
			{
				/* if not matched, print error messages and prompt from the beginning */
				LCD_displayStringRowColumn(0, 0, "Error!! ");
				LCD_displayStringRowColumn(1, 0, "NOT MATCHED");
			}
			TIMER1_delay_1sec();
		}
	}while(!matched); /* keep prompting for a correct password to be set */
}


/*
 * Description :
 * 		This function is responsible for prompting the user for correct passwords,
 * 		within maximum 3 times.
 * Return:
 * 			1 Password is correct.
 * 		   	0 All trials are used without password being correct
 */
uint8 checkPassword_trials(void)
{
	uint8 maxTrials = 3;
	uint8 isCorrect;

	while(maxTrials--)
	{

		/* prompt user to enter the system password to execute the required action,
		 * then check if password is correct
		 */
		isCorrect = verifyPass_ControlECU();

		if ('1' == isCorrect)
		{
			/* password is correct */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "ACCESS GRANTED");
			TIMER1_delay_1sec();
			return 1;
		}
		else{
		/* if password is false */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "ACCESS DENIED");
		TIMER1_delay_1sec();
		}
	}
	/* all 3 trials are used without password being correct */
	return 0;
}

/*
 * Description :
 * 			This function is responsible for executing the steps required to open the door
 */
void openDoor(void)
{
	int count_down = 3;
	/* Send a command to control_ECU to open the door */
	UART_sendByte('2');
	/* display opening message for 15 seconds */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is Unlocking");
	TIMER1_delay_15sec();

	/* display time remaining to lock the door */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door locks in");
	LCD_displayStringRowColumn(1, 8, "3");

	while(count_down--)
	{
		/* update count_down variable every 1 second */
		TIMER1_delay_1sec();
		LCD_moveCursor(1, 8);
		LCD_intgerToString(count_down);
	}

	/* display locking the door warning */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is locking  ");
	TIMER1_delay_15sec();
}

/*
 * Description :
 * 			This function is responsible for locking the systems when all password trials are used
 */
void lockSystem(void)
{
	int timer_counter = 0; /* used to repeat the 15sec delay function to get 1 min*/
	/* activate buzzer for 1 minute "send relative signal to control_mcu" */
	UART_sendByte('3');

	/* display error message on lcd for 1 minute */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "MAX TRIALS USED");
	LCD_displayStringRowColumn(1, 0, "SYSTEM IS LOCKED");
	/* no input received */

	/* Delay 1 minute */
	while(timer_counter++ < 4)
	{
		TIMER1_delay_15sec();
	}
}



/*
 * Description :
 * 		This function is to compare password entered by user to the saved password of the system
 * Return:
 * 			'1' Password is correct.
 * 		   	'0' Password is false.
 */
char verifyPass_ControlECU(void)
{
	uint8 response = 0;		/* flag that is set if entered password matches the system password */
	uint8 pass[10] = "";	/* to store the user entered password */
	uint8 pass_size = 0;	/* to indicate the user entered password size */

	/* prompt for password */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz enter pass:");
	LCD_moveCursor(1, 0);

	/* get user entered password */
	getPass(pass, &pass_size);

	/* send the password to the Control_ECU to be check with system password */
	UART_sendByte('1');
	_delay_ms(10);
	UART_sendString(pass);

	/* receive Control_ECU response */
	response = UART_recieveByte();
	return response;
}


/*
 * Description :
 * 		This function is responsible for storing the user entered password,
 * 		and printing '*' on LCD instead of each entered character
 */
void getPass(uint8 * passArr, uint8 * size)
{
	_delay_ms(100);
	*size = 0;
	do
	{
		passArr[(*size)++] = KEYPAD_getPressedKey();	/* store the entered keypad character in the array,
														   increment its size */
		if(passArr[(*size) - 1] != 13)
			LCD_displayCharacter('*');		/* print '*' on LCD in place of the entered keypad value, ignore ON key press */

		_delay_ms(250);				/* wait 100ms between two keypad presses */

		/* keep storing characters till ON key is pressed */
	}while(passArr[(*size) - 1] != 13); /* 13 is ASCII of Enter, returned by keypad if ON is pressed */

	passArr[--(*size)] = '\0'; /* terminate input string by null character, remove the enter character */

}

/*
 * Description :
 * 		This function is to compare passwords entered by user when setting a new password
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
	/* 118880 == 58594*/
	Timer1_Config_t config = { 1000 , 58594, TIMER1_PRESCALER_1024, TIMER1_CTC_MODE};
	Timer1_init(&config);
	Timer1_setCallBack(TIMER1_callback_function);
	while(ticks < 2);
	ticks = 0;
	Timer1_deInit();

}

/*
 * Description :
 * 			This function is to generate 1 second delay using timer1.
 */
void TIMER1_delay_1sec(void)
{
	/* required OCR value to generate 1 second at 1024 pre-scaler is 7813*/
	/* Timer will run in CTC, so we can put any dummy number in TCNT1 for the config*/
	Timer1_Config_t config = { 1000 , 7813, TIMER1_PRESCALER_1024, TIMER1_CTC_MODE};
	Timer1_init(&config);

	/* set timer callback function */
	Timer1_setCallBack(TIMER1_callback_function);
	/* wait for timer to generate the interrupt */
	while(!ticks);

	/* reset ticks variable for the next future use */
	ticks = 0;

	/* reset timer1 configurations */
	Timer1_deInit();

}

void TIMER1_callback_function(void)
{
	ticks++;
}



