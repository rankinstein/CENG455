/* ###################################################################
**     Filename    : client_task1.h
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-06, 17:22, # CodeGen: 4
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         client1_task - void client1_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file client_task1.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup client_task1_module client_task1 module documentation
**  @{
*/         

#ifndef __client_task1_H
#define __client_task1_H
/* MODULE client_task1 */

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


#ifdef __cplusplus
extern "C" {
#endif 

/*
** ===================================================================
**     Callback    : client1_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void client1_task(os_task_param_t task_init_data);


/* END client_task1 */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __client_task1_H*/
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
