/* ###################################################################
**     Filename    : serial_driver.c
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-08, 11:22, # CodeGen: 8
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_driver - void serial_driver(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file serial_driver.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup serial_driver_module serial_driver module documentation
**  @{
*/         
/* MODULE serial_driver */

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

/* declaration of global message pool */
_pool_id	message_pool;

/* declaration of the reading tasks list */
DEVICE_STRUCT open_read[MAX_DEVICES];

/* declaration of the writing task */
_task_id open_write = MQX_NULL_TASK_ID;

/* handler's queue id */
_queue_id	handler_qid;




/*
 * Public serial access functions: OpenR, _readline, OpenW, _putline, Close
 */

/* OpenR - Allow read access for a client */
bool OpenR(_mqx_uint stream_no) {
	/* obtain device_mutex */
	if(_mutex_lock(&read_access_mutex) != MQX_OK) {
		printf("\nError when trying to obtain device_mutex.\n");
		return FALSE;
	}

	/* get client task id from client_qid */
	_task_id client_task = _task_get_id();

	/* Check if task already has read access */
	for(int i = 0; i < MAX_DEVICES; i++) {
		if(open_read[i].client_id == client_task) {
			_mutex_unlock(&read_access_mutex);
			return FALSE;
		}
	}

	/* if the stream_no has not been given read access assign it */
	if(open_read[stream_no].client_id == 0) {

		/* get queue id from client queue number (stream_no) */
		_queue_id client_qid = _msgq_get_id(0, stream_no);
		if(client_qid == MSGQ_NULL_QUEUE_ID) {
			printf("\nError getting client queue id from queue number.\n");
			_mutex_unlock(&read_access_mutex);
			return FALSE;
		}

		open_read[stream_no] = (DEVICE_STRUCT) {
			.client_id = client_task,
			.client_qid = client_qid,
			.read_access = TRUE };

		_mutex_unlock(&read_access_mutex);
		//printf("device %d opened: %d, %d, %d\n", stream_no, open_read[stream_no].client_id, open_read[stream_no].client_qid, open_read[stream_no].read_access);
		return TRUE;
	}
	/* otherwise the device already has read access so return FALSE */
	else {
		_mutex_unlock(&read_access_mutex);
		return FALSE;
	}
}

/* OpenW - Request write access */
_queue_id OpenW() {
	/* obtain write access mutex */
	if(_mutex_lock(&write_access_mutex) != MQX_OK) {
		printf("\nError when trying to obtain write access mutex.\n");
		return (_queue_id) 0;
	}
	if(open_write == MQX_NULL_TASK_ID) {
		open_write = _task_get_id();
		_mutex_unlock(&write_access_mutex);
		return handler_qid;
	}
	_mutex_unlock(&write_access_mutex);
	return (_queue_id) 0;
}


/* INPROGRESS */
bool _putline(_queue_id qid, char* message) {
	/* task must have write permission */
	if(_mutex_lock(&write_access_mutex) != MQX_OK) {
		printf("\nError when trying to obtain write access mutex.\n");
		return FALSE;
	}
	_task_id client_id = _task_get_id();
	if(open_write != client_id) {
		_mutex_unlock(&write_access_mutex);
		return FALSE;
	}

	HANDLER_MESSAGE_PTR handler_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(message_pool);

	if (handler_ptr == NULL) {
		_mutex_unlock(&write_access_mutex);
		return FALSE;
	}
/* TODO
	handler_ptr->HEADER.SOURCE_QID = UNKNOWN_QUEUE;
	handler_ptr->HEADER.TARGET_QID = _msgq_get_id(0, HANDLER_QUEUE);
	handler_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + strlen((char *)handler_ptr->DATA) + 1;
	_mqx_uint i;
	for (i=0; i < strlen(message); i++) {
		handler_ptr->DATA[i] = myRxBuff[i];
	}
*/
	_mutex_unlock(&write_access_mutex);
	return TRUE;
}

/* Close - Remove write and/or read access from a task */
bool Close() {
	/* Check and revoke write access */
	bool write_revoked = FALSE;
	/* obtain write access mutex */
	if(_mutex_lock(&write_access_mutex) != MQX_OK) {
		printf("\nError when trying to obtain write access mutex.\n");
		return FALSE;
	}
	if(open_write == _task_get_id()) {
		open_write = MQX_NULL_TASK_ID;
		write_revoked = TRUE;
	}
	_mutex_unlock(&write_access_mutex);

	/* Check and revoke read access */
	bool read_revoked = FALSE;
	/* obtain read access mutex */
	if(_mutex_lock(&read_access_mutex) != MQX_OK) {
		printf("\nError when trying to obtain read access mutex.\n");
		return FALSE;
	}
	/* check read access list for the task id */
	for(int i = 0; i < MAX_DEVICES; i++) {
		if(open_read[i].client_id == _task_get_id()) {
			open_read[i] = (DEVICE_STRUCT) {0,0,0};
			read_revoked = TRUE;
			break;
		}
	}
	_mutex_unlock(&read_access_mutex);

	return write_revoked || read_revoked;
}

/*
 * End of public serial access functions
 */



/* Handle character input */
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
**     Callback    : serial_driver
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_driver(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	printf("serialTask Created\n\r");

		HANDLER_MESSAGE_PTR handler_ptr;
		_mqx_uint	i;

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

		/* initialize opened_devices */
		for(i = 0; i < MAX_DEVICES; i++) {
			open_read[i] = (DEVICE_STRUCT) {0,0,0};
		}

		/* initialize read_access_mutex */
		MUTEX_ATTR_STRUCT read_mutexattr;
		if(_mutatr_init(&read_mutexattr) != MQX_OK) {
			printf("\nCould not initialize read mutex attributes.\n");
			_task_block();
		}
		if(_mutex_init(&read_access_mutex, &read_mutexattr) != MQX_OK){
			printf("\nCould not create read access mutex.\n");
			_task_block();
		}

		/* initialize write_access_mutex */
		MUTEX_ATTR_STRUCT write_mutexattr;
			if(_mutatr_init(&write_mutexattr) != MQX_OK) {
				printf("\nCould not initialize write mutex attributes.\n");
				_task_block();
			}
			if(_mutex_init(&write_access_mutex, &write_mutexattr) != MQX_OK){
				printf("\nCould not create write access mutex.\n");
				_task_block();
			}

		/*
		 * Handler consume message loop
		 */
		while (TRUE) {
			handler_ptr = _msgq_receive(handler_qid, 0);

			if (handler_ptr == NULL) {
				printf("\nCould not receive a message\n");
				_task_block();
			}

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
		/*
		 * end of consume message loop
		 */

		unsigned char buf[13];
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

/* END serial_driver */

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
