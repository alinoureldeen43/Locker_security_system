/******************************************************************************
 *
 * Module: APP
 *
 * File Name: app.h
 *
 * Description: Header file for the application
 *
 * Author: Ali Hassan
 *
 *******************************************************************************/

#ifndef APP_H_
#define APP_H_

#include "../std_types.h"


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :
 * This function is responsible for initializing the peripherals used
 */
void APP_init(void);

/*
 * Description :
 * This function is responsible for operating the system as requried
 */
void APP_start(void);

#endif /* APP_H_ */
