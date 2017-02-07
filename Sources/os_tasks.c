/* ###################################################################
**     Filename    : os_tasks.c
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
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "client_task1.h"
#include "client_task2.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* declaration of global message pool */
_pool_id	message_pool;

/* User includes (#include below this line is not maintained by Processor Expert) */

void handle_char(unsigned char c) {
	if (c==0x08) {
		printf("got a backspace\n");
	} else if (c==0x17) {
		printf("got a ctrl+w (back a word).\n");
	} else if (c == 0x15) {
		printf("got a ctrl+u (back a whole line).\n");
	} else {
		printf("%c", c);
	}
}

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  
	printf("serialTask Created\n\r");

	HANDLER_MESSAGE_PTR handler_ptr;
	_mqx_uint	i;
	_queue_id	handler_qid;
	bool		result;

	/* open a message queue */
	handler_qid = _msgq_open(HANDLER_QUEUE, 0);

	if (handler_qid == 0) {
		printf("\nCould not open the server message queue.\n");
		_task_block();
	}

	/* create a message pool */
	message_pool = _msgpool_create(sizeof(HANDLER_MESSAGE), NUM_HANDLER_MESSAGES, 0, 0);

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCould not create a message pool\n");
		_task_block();
	}

	while (TRUE) {
		handler_ptr = _msgq_receive(handler_qid, 0);

		if (handler_ptr == NULL) {
			printf("\nCould not receive a message\n");
			_task_block();
		}

		_time_delay_ticks(1);

		if (handler_ptr->HEADER.SOURCE_QID == ISR_QUEUE) {
			handle_char(handler_ptr->DATA[0]);
			/* switch statement for handling individual messages */
		} else {
			printf("\nGot a message not from the ISR.\n");
			printf("%d\n", handler_ptr->HEADER.SOURCE_QID);
			/* switch statement for handling individual messages */
		}

		_msg_free(handler_ptr);
	}

	char buf[13];
	sprintf(buf, "\n\rType here: ");
	UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END os_tasks */

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
