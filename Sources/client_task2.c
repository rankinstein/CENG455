/* ###################################################################
**     Filename    : client_task2.c
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-06, 17:24, # CodeGen: 5
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         client2_task - void client2_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file client_task2.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup client_task2_module client_task2 module documentation
**  @{
*/         
/* MODULE client_task2 */

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
**     Callback    : client2_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void client2_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	printf("clientTask2 Created\n\r");

	HANDLER_MESSAGE_PTR client2_ptr;
	_mqx_uint	i;
	_queue_id	client2_qid;
	bool		result;

	/* open a message queue */
	client2_qid = _msgq_open(CLIENT2_QUEUE, 0);

	if (client2_qid == 0) {
		printf("\nCould not open client2 queue.\n");
		_task_block();
	}

	printf("\nClient access tests:");
	result = OpenR(CLIENT2_QUEUE);
	printf("\nTEST 1 - Can open read access: %s", result ? "PASS":"FAIL");

	result = OpenR(MASTER_QUEUE);
	printf("\nTEST 2 - Re-open read access returns false: %s", result == 0 ? "PASS":"FAIL");

	_queue_id handler = OpenW();
	printf("\nTEST 3 - Cannot open write access because it is opened by master: %s", handler == 0 ? "PASS":"FAIL");

	result = Close();
	printf("\nTEST 4 - Close response is true: %s", result ? "PASS":"FAIL");

	result = Close();
	printf("\nTEST 5 - Closing when task has neither read or write returns false: %s", result == FALSE ? "PASS":"FAIL");

	result = OpenR(CLIENT2_QUEUE);
	printf("\nTEST 6 - After closing can open read access: %s", result ? "PASS":"FAIL");

	handler = OpenW();
	printf("\nTEST 7 - Still cannot open write access because it is still held by master: %s", handler == 0 ? "PASS":"FAIL");

	printf("\nEnd of client access tests\n");

	/* Client 2 opens read access every 5 seconds */
	char* message;
	while (TRUE) {
		/* get a line from the serial channel */
		result = _getline(&message);
		if(!result) {
			printf("\nError reading message\n");
		}
		printf("Client sees: %s\n", message);
		Close();
		_time_delay(5000);
		result = OpenR(CLIENT2_QUEUE);
		if(!result) {
			printf("\nError with Client 2 opening read access.\n");
		}

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

/* END client_task2 */

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
