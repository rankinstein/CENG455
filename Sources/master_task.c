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
	printf("Master Created\n\r");

	HANDLER_MESSAGE_PTR client1_ptr;
	_mqx_uint			i;
	_queue_id			master_qid;
	_queue_id 			serial_driver_qid;
	bool				result;
	char*				message;

	/* open a message queue */
	master_qid = _msgq_open(MASTER_QUEUE, 0);
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

	/* Open read access to serial driver */
	result = OpenR(MASTER_QUEUE);
	if(!result) {
		printf("\nCould not establish read access to serial driver.\n");
		_task_block();
	}

	/* Open write access to serial driver */
	serial_driver_qid = OpenW();
	if(serial_driver_qid == 0) {
		printf("\nCould not establish write access to the serial driver.\n");
	}

	/* Master reads input from the serial channel and echos it back */
	while(TRUE) {
		/* get a line from the serial channel */
		result = _getline(&message);
		if(!result) {
			printf("\nError reading message\n");
		}

		/* put the line back to the serial channel */
		result = _putline(serial_driver_qid, message);
		if(!result) {
			printf("\nError putting message\n");
		}
	}


#ifdef PEX_USE_RTOS
  while (1) {
#endif
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
