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

	printf("\nClient method tests:");
	bool r_access = OpenR(CLIENT2_QUEUE);
	printf("\nTEST 1 - Can open read access: %s", r_access ? "PASS":"FAIL");

	r_access = OpenR(CLIENT1_QUEUE);
	printf("\nTEST 2 - Re-open read access returns false: %s", r_access == 0 ? "PASS":"FAIL");

	_queue_id handler = OpenW();
	printf("\nTEST 3 - Can open write access: %s", handler != 0 ? "PASS":"FAIL");

	handler = OpenW();
	printf("\nTEST 4 - Re-open read access returns 0: %s", handler == 0 ? "PASS":"FAIL");

	bool c_resp = Close();
	printf("\nTEST 5 - Close response is true: %s", c_resp ? "PASS":"FAIL");

	c_resp = Close();
	printf("\nTEST 6 - Closing when task has neither read or write returns false: %s", c_resp == FALSE ? "PASS":"FAIL");

	r_access = OpenR(CLIENT2_QUEUE);
	printf("\nTEST 7 - After closing can open read access: %s", r_access ? "PASS":"FAIL");

	handler = OpenW();
	printf("\nTEST 8 - After closing can open write access: %s", handler ? "PASS":"FAIL");

	/* incomplete - testing _putline() */
	char message[8];
	message[0] = 't';
	message[1] = 'e';
	message[2] = 's';
	message[3] = 't';
	message[4] = '\0';
	bool put_response = _putline(handler, &message);
	printf("\nput response: %d\n", put_response);
	/* end of incomplete */

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
