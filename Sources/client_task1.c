/* ###################################################################
**     Filename    : client_task1.c
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
** @file client_task1.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup client_task1_module client_task1 module documentation
**  @{
*/         
/* MODULE client_task1 */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "serial_driver.h"
#include "client_task1.h"
#include "client_task2.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : client1_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void client1_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  
	printf("clientTask1 Created\n\r");

	HANDLER_MESSAGE_PTR client1_ptr;
	_mqx_uint	i;
	_queue_id	client1_qid;
	bool		result;

	/* open a message queue */
	client1_qid = _msgq_open(CLIENT1_QUEUE, 0);

	if (client1_qid == 0) {
		printf("\nCould not open client1 queue.\n");
		_task_block();
	}


#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END client_task1 */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
