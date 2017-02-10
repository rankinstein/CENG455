/* ###################################################################
**     Filename    : serial_driver.h
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-08, 11:22, # CodeGen: 8
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_driver - void serial_driver(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file serial_driver.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup serial_driver_module serial_driver module documentation
**  @{
*/         

#ifndef __serial_driver_H
#define __serial_driver_H
/* MODULE serial_driver */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "mqx_ksdk.h"
#include "uart1.h"
#include "fsl_mpu1.h"
#include "fsl_hwtimer1.h"
#include "MainTask.h"
#include "serialDriver.h"
#include "myUART.h"
#include "master.h"
#include "client2.h"

#ifdef __cplusplus
extern "C" {
#endif 

#include <message.h>
#include <mutex.h>
/*
 * serial driver queue numbers's
 */
#ifndef SERIAL_UNDEFINED_QUEUE
#define SERIAL_UNDEFINED_QUEUE  	7 // used when the message source is unknown
#endif
#ifndef SERIAL_HANDLER_QUEUE
#define SERIAL_HANDLER_QUEUE		8
#endif
#ifndef SERIAL_ISR_QUEUE
#define SERIAL_ISR_QUEUE			9
#endif


/*
 * max number of clients who can open read or write access
 * This value must be greater than or equal to the largest queue number
 * of the tasks that request read access
 */
#define MAX_DEVICES			20


/*
 * max # of waiting messages
 */
#define NUM_HANDLER_MESSAGES	(10)


/*
 * max # of data characters in a handler message
 */
#define HANDLER_MESSAGE_SIZE 256


/*
 * structures
 */
typedef struct handler_message
{
	MESSAGE_HEADER_STRUCT HEADER;
	unsigned char DATA[HANDLER_MESSAGE_SIZE];
} HANDLER_MESSAGE, * HANDLER_MESSAGE_PTR;

typedef struct device
{
	_task_id client_id;
	_queue_id client_qid;
	bool read_access;
} DEVICE_STRUCT;
/*
 * end of structures
 */

/*
 * mutexes
 */
MUTEX_STRUCT read_access_mutex;
MUTEX_STRUCT write_access_mutex;
/*
 * end of mutexes
 */

/* serial driver message pool */
extern _pool_id message_pool;


/*
 * Public serial driver access functions
 */
extern bool OpenR(_mqx_uint stream_no);
extern bool _getline(char** message);
extern _queue_id OpenW();
extern bool _putline(_queue_id, char*);
extern bool Close();
/*
 * end of public access functions
 */


/*
** ===================================================================
**     Callback    : serial_driver
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_driver(os_task_param_t task_init_data);


/* END serial_driver */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __serial_driver_H*/
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
