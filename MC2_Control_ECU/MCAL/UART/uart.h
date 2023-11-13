 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Ali Hassan
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "../../std_types.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
typedef enum
{
	UART_5_DATA_BITS,
	UART_6_DATA_BITS,
	UART_7_DATA_BITS,
	UART_8_DATA_BITS,
}UART_BitData;

typedef enum
{
	UART_PARITY_DISABLED,
	UART_PARITY_EVEN = 2,
	UART_PARITY_ODD
}UART_Parity;

typedef enum
{
	UART_1_STOP_BIT,
	UART_2_STOP_BITS
}UART_StopBit;

typedef uint16 UART_BaudRate;

typedef struct
{
	UART_BitData bit_data;
	UART_Parity parity;
	UART_StopBit stop_bit;
	UART_BaudRate baud_rate;
}UART_Config_t;
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(UART_Config_t * config);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
