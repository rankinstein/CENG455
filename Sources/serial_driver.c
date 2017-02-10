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

/* handler data buffer */
char handler_buffer[HANDLER_MESSAGE_SIZE];
/* cursor position */
int handler_cursor_position = 0;


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
		return TRUE;
	}
	/* otherwise the device already has read access so return FALSE */
	else {
		_mutex_unlock(&read_access_mutex);
		return FALSE;
	}
}

/* _readline - Wait for input from the serial channel */
bool _getline(char** message) {
	/* check if task has read access */
	bool has_read_access = FALSE;
	/* obtain device_mutex */
	if(_mutex_lock(&read_access_mutex) != MQX_OK) {
		printf("\nError when trying to obtain device_mutex.\n");
		return FALSE;
	}

	/* get client task id from client_qid */
	_task_id client_task = _task_get_id();
	_queue_id client_qid;

	/* Check if task already has read access */
	for(int i = 0; i < MAX_DEVICES; i++) {
		if(open_read[i].client_id == client_task) {
			has_read_access = TRUE;
			client_qid = open_read[i].client_qid;
			break;
		}
	}
	_mutex_unlock(&read_access_mutex);
	if(!has_read_access) {
		return FALSE;
	}

	HANDLER_MESSAGE_PTR message_ptr = _msgq_receive(client_qid, 0);
	if (message_ptr == NULL) {
		printf("\nCould not receive a message\n");
		return FALSE;
	}

	*message = message_ptr->DATA;
	_msg_free(message_ptr);
	return TRUE;
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


/* _putline - Send message to serial port */
bool _putline(_queue_id qid, char* message_ptr) {
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

	handler_ptr->HEADER.SOURCE_QID = SERIAL_UNDEFINED_QUEUE;
	handler_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SERIAL_HANDLER_QUEUE);
	handler_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char) * HANDLER_MESSAGE_SIZE;
	_mqx_uint i;
	for (i=0; i < strlen(message_ptr); i++) {
		handler_ptr->DATA[i] = message_ptr[i];
	}
	handler_ptr->DATA[i] = '\n';
	handler_ptr->DATA[i+1] = '\r';
	handler_ptr->DATA[i+2] = '\0';

	bool result = _msgq_send(handler_ptr);
	if (result != TRUE) {
		_mutex_unlock(&write_access_mutex);
		return FALSE;
	}

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

/* Go back one character */
void go_back_char() {
	unsigned char char_array[4];
	char_array[0] = '\b';
	char_array[1] = ' ';
	char_array[2] = '\b';
	char_array[3] = '\0';
	UART_DRV_SendDataBlocking(myUART_IDX, char_array, sizeof(char) * strlen(char_array), 1000);

	handler_buffer[handler_cursor_position--] = '\0';
}

void send_message_to_readers() {
	/* Check and revoke read access */
	/* obtain read access mutex */
	if(_mutex_lock(&read_access_mutex) != MQX_OK) {
		printf("\nError when trying to obtain read access mutex.\n");
		return;
	}

	_mqx_uint broadcast_ctr = 0;
	_queue_id broadcast_to[MAX_DEVICES];
	/* check read access list for the task id */
	for(int i = 0; i < MAX_DEVICES; i++) {
		if(open_read[i].client_id != 0) {
			broadcast_to[broadcast_ctr++] = open_read[i].client_qid;
			//create message and send
		}
	}
	_mutex_unlock(&read_access_mutex);

	if(broadcast_ctr == 0){
		return;
	}

	broadcast_to[broadcast_ctr] = MSGPOOL_NULL_POOL_ID;

	HANDLER_MESSAGE_PTR handler_ptr = (HANDLER_MESSAGE_PTR)_msg_alloc(message_pool);
	handler_ptr->HEADER.SOURCE_QID = _msgq_get_id(0, SERIAL_HANDLER_QUEUE);
	handler_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char) * HANDLER_MESSAGE_SIZE;
	_mqx_uint i;
	for (i=0; i < handler_cursor_position; i++) {
		handler_ptr->DATA[i] = handler_buffer[i];
	}
	handler_ptr->DATA[i] = '\0';

	_mqx_uint message_result = _msgq_send_broadcast(handler_ptr, broadcast_to, message_pool);

	return;
}

/* Handle character input */
void handle_char(unsigned char c) {
	if (c==0x08) {
		if (handler_cursor_position > 0) {
			go_back_char();
		}
	} else if (c==0x17) {
		bool seen_letter = FALSE;
		while(handler_cursor_position > 0) {
			if (seen_letter && handler_buffer[handler_cursor_position-1] == ' ') {
				break;
			}
			if (handler_buffer[handler_cursor_position-1] != ' ') {
				seen_letter = TRUE;
			}
			go_back_char();
		}
	} else if (c == 0x15) {
		while(handler_cursor_position > 0) {
			go_back_char();
		}
	} else if (c == 0x0d) {
		// send to listeners
		send_message_to_readers();
		// output \n\r
		unsigned char char_array[3];
		char_array[0] = '\n';
		char_array[1] = '\r';
		char_array[2] = '\0';
		UART_DRV_SendDataBlocking(myUART_IDX, char_array, sizeof(char) * strlen(char_array), 1000);
		// reset cursor and handler buffer
		handler_buffer[0] = '\0';
		handler_cursor_position = 0;
	} else {
		handler_buffer[handler_cursor_position++] = c;
		printf("%c", c);
		unsigned char char_array[2];
		char_array[0] = c;
		char_array[1] = '\0';
		UART_DRV_SendDataBlocking(myUART_IDX, char_array, sizeof(char) * strlen(char_array), 1000);
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

		HANDLER_MESSAGE_PTR		handler_ptr;
		_mqx_uint				i;
		bool					result;


		/* open a message queue */
		handler_qid = _msgq_open(SERIAL_HANDLER_QUEUE, 0);

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
			/* Consume the next message in the queue */
			handler_ptr = _msgq_receive(handler_qid, 0);
			if (handler_ptr == NULL) {
				printf("\nCould not receive a message\n");
				_task_block();
			}

			/* If the message is from the ISR handle the input characters */
			if (handler_ptr->HEADER.SOURCE_QID == SERIAL_ISR_QUEUE) {
				for(i=0; handler_ptr->DATA[i] != '\0'; i++) {
					handle_char(handler_ptr->DATA[i]);
				}
			}
			/* Otherwise the message is from _putline() */
			else {
				UART_DRV_SendDataBlocking(myUART_IDX, handler_ptr->DATA, sizeof(char) * strlen(handler_ptr->DATA), 1000);
			}

			_msg_free(handler_ptr);
		}
		/*
		 * end of consume message loop
		 */

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
