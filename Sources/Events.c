/* ###################################################################
**     Filename    : Events.c
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-01-30, 16:02, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

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
**     Callback    : myUART_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The UART instance number.
**       uartState - A pointer to the UART driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/
void myUART_RxCallback(uint32_t instance, void * uartState)
{
	/* create a message */
	HANDLER_MESSAGE_PTR 	handler_ptr;

	handler_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(message_pool);

	if (handler_ptr == NULL) {
		return;
	}

	handler_ptr->HEADER.SOURCE_QID = SERIAL_ISR_QUEUE;
	handler_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SERIAL_HANDLER_QUEUE);
	handler_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + strlen((char *)handler_ptr->DATA) + 1;
	for (_mqx_uint i=0; i<sizeof(myRxBuff); i++) {
		handler_ptr->DATA[i] = myRxBuff[i];
	}

	bool result = _msgq_send(handler_ptr);

	if (result != TRUE) {
		return;
	}

	return;
}

/* END Events */

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
