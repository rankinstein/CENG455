/* ###################################################################
**     Filename    : os_tasks.h
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-01-30, 16:14, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_task - void serial_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         

#ifndef __os_tasks_H
#define __os_tasks_H
/* MODULE os_tasks */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "mqx_ksdk.h"
#include "uart1.h"
#include "fsl_mpu1.h"
#include "fsl_hwtimer1.h"
#include "MainTask.h"
#include "serialTask.h"
#include "myUART.h"
#include "client1.h"
#include "client2.h"

#include <message.h>
#include <mutex.h>


#ifdef __cplusplus
extern "C" {
#endif 

/*
 * queue numbers's
 */
#define HANDLER_QUEUE	8
#define ISR_QUEUE		9
#define CLIENT1_QUEUE	10
#define CLIENT2_QUEUE	11
/*
 * end of queue numbers's
 */

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
 * structures
 */
typedef struct handler_message
{
	MESSAGE_HEADER_STRUCT HEADER;
	unsigned char DATA[256];
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

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data);

extern _pool_id message_pool;
extern bool OpenR(_mqx_uint stream_no);
extern _queue_id OpenW();
extern bool Close();

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __os_tasks_H*/
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
