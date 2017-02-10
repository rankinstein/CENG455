/* ###################################################################
**     Filename    : master_task.c
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-08, 11:39, # CodeGen: 9
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         master_task - void master_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file master_task.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup master_task_module master_task module documentation
**  @{
*/         
/* MODULE master_task */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "serial_driver.h"
#include "master_task.h"
#include "client_task2.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : master_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void master_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */

	printf("Master Created\n\r");

	HANDLER_MESSAGE_PTR client1_ptr;
	_mqx_uint	i;
	_queue_id	master_qid;
	bool		result;

	/* open a message queue */
	master_qid = _msgq_open(CLIENT1_QUEUE, 0);

	if (master_qid == 0) {
		printf("\nCould not open client1 queue.\n");
		_task_block();
	}

	/* Start the serial driver task */
	_task_id serial_driver_id = _task_create(0, SERIALDRIVER_TASK, (uint32_t)(NULL));
	if (serial_driver_id == MQX_NULL_TASK_ID) {
		printf("\nCould not start the serial driver.\n");
		_task_block();
	}

	bool r_access = OpenR(CLIENT1_QUEUE);
	while (TRUE) {
		HANDLER_MESSAGE_PTR handler_ptr = _msgq_receive(master_qid, 0);

		if (handler_ptr == NULL) {
			printf("\nCould not receive a message\n");
			_task_block();
		}

		printf("Master sees: %s\n", handler_ptr->DATA);

		_msg_free(handler_ptr);
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

/* END master_task */

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
