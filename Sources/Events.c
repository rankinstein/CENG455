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
#include "client_task1.h"
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
  /* Write your code here ... */
	UART_DRV_SendData(myUART_IDX, myRxBuff, sizeof(myRxBuff));
	/* create a message */
	HANDLER_MESSAGE_PTR 	handler_ptr;
	bool					result;
	_mqx_uint				i;

	handler_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(message_pool);

	if (handler_ptr == NULL) {
		return;
	}

	handler_ptr->HEADER.SOURCE_QID = ISR_QUEUE;
	handler_ptr->HEADER.TARGET_QID = _msgq_get_id(0, HANDLER_QUEUE);
	handler_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + strlen((char *)handler_ptr->DATA) + 1;
	for (i=0; i<sizeof(myRxBuff); i++) {
		handler_ptr->DATA[i] = myRxBuff[i];
	}

	result = _msgq_send(handler_ptr);

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
