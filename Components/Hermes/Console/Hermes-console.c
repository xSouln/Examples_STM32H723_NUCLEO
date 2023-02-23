//==============================================================================
//includes:

#include "Hermes-console.h"
#include "hermes.h"

#include "Common/xCircleBuffer.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"
#include "cmsis_os.h"

#include "usart.h"
//==============================================================================
//defines:

#define HERMES_CONSOL_RX_BUFFER_SIZE_MASK 0x3ff
#define HERMES_CONSOL_RX_BUFFER_SIZE (HERMES_CONSOL_RX_BUFFER_SIZE_MASK + 1)

#define HERMES_CONSOL_TX_BUFFER_SIZE_MASK 0x1fff
#define HERMES_CONSOL_TX_BUFFER_SIZE (HERMES_CONSOL_TX_BUFFER_SIZE_MASK + 1)
//==============================================================================
//externs:

extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
//==============================================================================
//overrides:


//==============================================================================
//variables:

//------------------------------------------------------------------------------
TaskHandle_t hermes_consol_tx_task_handle;
StaticTask_t hermes_consol_tx_task_object HERMES_CONSOL_TX_TASK_STACK_MEM_SECTION;
StackType_t hermes_consol_tx_task_stack[HERMES_CONSOL_TX_TASK_SIZE] HERMES_CONSOL_TX_TASK_STACK_MEM_SECTION;

TaskHandle_t hermes_consol_task_handle;
StaticTask_t hermes_consol_task_object HERMES_CONSOL_TASK_STACK_MEM_SECTION;
StackType_t hermes_consol_task_stack[HERMES_CONSOL_TASK_SIZE] HERMES_CONSOL_TASK_STACK_MEM_SECTION;

SemaphoreHandle_t hermes_consol_tx_semaphore;

SemaphoreHandle_t hermes_consol_write_mutex = 0;
SemaphoreHandle_t hermes_consol_read_mutex = 0;
//------------------------------------------------------------------------------
static uint8_t hermes_console_rx_buffer[HERMES_CONSOL_RX_BUFFER_SIZE] HERMES_CONSOL_RX_BUFFER_MEM_LOCATION;
static uint8_t hermes_console_tx_buffer[HERMES_CONSOL_TX_BUFFER_SIZE] HERMES_CONSOL_TX_BUFFER_MEM_LOCATION;

static xCircleBufferT RxCircleBuffer;
static xCircleBufferT TxCircleBuffer;

static REG_UART_T* Usart;

static char consol_output[256];
static char consol_input[256];
//==============================================================================
//prototypes:


//==============================================================================
//functions:

inline static void HERMES_CONSOL_WRITE_BLOCK()
{
	xSemaphoreTake(hermes_consol_write_mutex, portMAX_DELAY);
}
//------------------------------------------------------------------------------
inline static void HERMES_CONSOL_WRITE_UNBLOCK()
{
	xSemaphoreGive(hermes_consol_write_mutex);
}
//------------------------------------------------------------------------------
inline static void HERMES_CONSOL_READ_BLOCK()
{
	xSemaphoreTake(hermes_consol_read_mutex, portMAX_DELAY);
}
//------------------------------------------------------------------------------
inline static void HERMES_CONSOL_READ_UNBLOCK()
{
	xSemaphoreGive(hermes_consol_read_mutex);
}
//------------------------------------------------------------------------------
int HermesConsoleRead(void* out, uint16_t size, ConsoleWriteModes mode)
{
	HERMES_CONSOL_READ_BLOCK();

	uint8_t* data = out;

	while (size)
	{
		RxCircleBuffer.TotalIndex = (RxCircleBuffer.SizeMask + 1)
				- ((DMA_Stream_TypeDef*)hdma_usart3_rx.Instance)->NDTR;

		uint16_t count = xCircleBufferRead(&RxCircleBuffer, data, size);

		data += count;
		size -= count;
	}

	HERMES_CONSOL_READ_UNBLOCK();

	return 0;
}
//------------------------------------------------------------------------------
int HermesConsoleWrite(void* in, uint16_t size, ConsoleWriteModes mode)
{
	HERMES_CONSOL_WRITE_BLOCK();

	xCircleBufferAdd(&TxCircleBuffer, (uint8_t*)in, size);

	HERMES_CONSOL_WRITE_UNBLOCK();

	xSemaphoreGive(hermes_consol_tx_semaphore);

	return 0;
}
//------------------------------------------------------------------------------
int HermesConsoleWriteString(char* in)
{
	return HermesConsoleWrite(in, strlen(in), 0);
}
//------------------------------------------------------------------------------
static int HermesConsoleMessageCheck(HERMES_MESSAGE_RECEIVED* incomingMessage, uint32_t bytesReceived)
{
	//We havent even got enough bytes to match the header
	if(bytesReceived < sizeof(HERMES_UART_HEADER))
	{
		return DEAL_NOT_ENOUGH_BYTES;
	}

	if((incomingMessage->header_in.check + incomingMessage->header_in.type) != 0xFF)
	{
		return DEAL_INVALID_HEADER;
	}

	if(incomingMessage->header_in.type > MESSAGE_BACKSTOP)
	{
		return DEAL_INVALID_HEADER;
	}

	if(incomingMessage->header_in.size + sizeof(HERMES_UART_HEADER) > bytesReceived)
	{
		return DEAL_NOT_ENOUGH_BYTES;
	}

	return (sizeof(HERMES_UART_HEADER) + incomingMessage->header_in.size);
}
//------------------------------------------------------------------------------
static int HermesConsoleReadLine(uint8_t *buf, size_t size)
{
	static char incomingUARTBuffer[sizeof(HERMES_MESSAGE_RECEIVED)] __attribute__((section("._user_dtcmram_ram")));
	static uint32_t	bytesIntoIncomingUARTBufferCounter = 0;

	// this is in the zeroed section.
	static uint8_t old_buf[40] __attribute__((section("._user_dtcmram_ram")));

	extern bool shouldIPackageFlag;
	extern bool shouldICryptFlag;

	char ch;
	int	i = 0;
	char delstr[] = { 0x20, 0x08, 0x00 };
	int isHeaderLegitAndHowBigIsIt = 0;
	uint32_t getting_escape_seq	= 0;
	bool escape_seq_found = false;

	assert(buf != NULL);

	if(shouldIPackageFlag)
	{
		//if we should package is true, we have a message with a header, lets check that bombay bad boi out
		while(true)
		{
			//Read the message here
			while(HermesConsoleRead(incomingUARTBuffer + bytesIntoIncomingUARTBufferCounter, 1, 0))
			{
				bytesIntoIncomingUARTBufferCounter++;

				isHeaderLegitAndHowBigIsIt = HermesConsoleMessageCheck((HERMES_MESSAGE_RECEIVED*)incomingUARTBuffer, bytesIntoIncomingUARTBufferCounter);

				if(isHeaderLegitAndHowBigIsIt > 0)
				{
					//successful message but we need to clip it by isHeaderLegitAndHowBigIsIt
					if(shouldICryptFlag == false)
					{
						HERMES_MESSAGE_RECEIVED* hermes_message_received = (HERMES_MESSAGE_RECEIVED*)incomingUARTBuffer;

						memcpy(buf, hermes_message_received->payload, hermes_message_received->header_in.size);
						buf[hermes_message_received->header_in.size] = '\0';
						memcpy(incomingUARTBuffer, &incomingUARTBuffer[isHeaderLegitAndHowBigIsIt], sizeof(incomingUARTBuffer) - isHeaderLegitAndHowBigIsIt);
						bytesIntoIncomingUARTBufferCounter -= isHeaderLegitAndHowBigIsIt;

						return hermes_message_received->header_in.size;
					}
				}
				else
				{
					if(bytesIntoIncomingUARTBufferCounter >= strlen(GREETING_STRING))
					{

						if(strncmp(GREETING_STRING,incomingUARTBuffer,strlen(GREETING_STRING)) == 0)
						{
							incomingUARTBuffer[bytesIntoIncomingUARTBufferCounter] = '\0';
							char* returnLocation = strchr(incomingUARTBuffer,'\r');

							if(returnLocation != NULL)
							{
								uint32_t commandLength = (uint32_t)(returnLocation-incomingUARTBuffer);
								memcpy(buf, incomingUARTBuffer,commandLength);
								buf[commandLength] = '\0';

								//sprintf((char*)buf, "%s", GREETING_STRING);
								memcpy(incomingUARTBuffer, &incomingUARTBuffer[commandLength+1], sizeof(incomingUARTBuffer) - commandLength + 1);
								bytesIntoIncomingUARTBufferCounter -= commandLength + 1;
								return commandLength;
							}
						}
					}

					if(isHeaderLegitAndHowBigIsIt == DEAL_INVALID_HEADER)
					{
						//remove the first byte
						memcpy(incomingUARTBuffer, &incomingUARTBuffer[1], --bytesIntoIncomingUARTBufferCounter);
					}
				}
			}
		}
	}

	if(shouldICryptFlag)
	{
		//decrypt the message
	}

	//sort out message here
	//message_received.header_in.type = MESSAGE_COMMAND;

	while((i < size)
	&& (HermesConsoleRead(&ch, 1, 0))
	&& (ch != '\r')
	&& (escape_seq_found == false));
	{
		switch(getting_escape_seq)
		{
			case 0:
				// no escape sequence, so normal operation

				switch(ch)
				{
					case 0x1b:
						// escape code - start of cursor keys
						getting_escape_seq = 1;
						break;

					case 0x8:
						// backspace
					case 0x7f:
						// delete
						if(i > 0)
						{
							//LOG_EchoCharacter((uint8_t *)&ch, false, &i);
							HermesConsoleWrite(delstr, 2, ConsoleWriteCommonMode);
							i--;
						}
						break;

					case '\r':
					case '\n':
						// need to drop these
						break;

					default:
						if(i < (size-1))
						{
							//LOG_EchoCharacter((uint8_t*)&ch, false, &i);
							buf[i++] = ch;
						}
						break;
				}
				break;

			case 1:
				// got the first char of the escape sequence, so look for the 2nd
				switch(ch)
				{
					case 0x5b:
						// 2nd part of escape sequence
						getting_escape_seq = 2;
						break;

					default:
						getting_escape_seq = 0;
						break;
				}
				break;

			case 2:
				// got both of the first two chars, so look for the actual result
				switch(ch)
				{
					case 'A':
						// uparrow
						escape_seq_found = true;
						if(strlen((char const*)old_buf) > 0)
						{
							strcpy((char*)buf, (const char*)old_buf);
							i = strlen((char const*)buf);
							HermesConsoleWrite(buf, i, ConsoleWriteCommonMode);
						}
						break;

					case 'B':
						// downarrow
					case 'C':
						// rightarrow
					case 'D':
						// leftarrow
					default:
						break;
				}
				// ready for next time
				getting_escape_seq = 0;
				break;
		}
	}

	// Add a terminator
	buf[i] = '\0';

	if(strlen((const char*)buf) < (sizeof(old_buf) - 1))
	{
		// keep the old version in case we use uparrow
		strcpy((char*)old_buf, (const char*)buf);
	}

	return i;
}
//------------------------------------------------------------------------------
void HermesConsoleFlush()
{
	while (!xCircleBufferIsEmpty(&TxCircleBuffer) && Usart->Control1.TxEmptyInterruptEnable)
	{
		osThreadYield();
	}
}
//------------------------------------------------------------------------------

void HermesConsoleUART_IRQ()
{
	if (Usart->Control1.TxEmptyInterruptEnable && Usart->InterruptAndStatus.TxEmpty)
	{
		if (!xCircleBufferIsEmpty(&TxCircleBuffer))
		{
			Usart->TxData = xCircleBufferGet(&TxCircleBuffer);
		}
		else
		{
			Usart->Control1.TxEmptyInterruptEnable = false;
		}
	}
}
//------------------------------------------------------------------------------
void HermesConsoleTxTask(void* arg)
{
	while(true)
	{
		xSemaphoreTake(hermes_consol_tx_semaphore, 10);

		if(!Usart->Control1.TxEmptyInterruptEnable
		&& TxCircleBuffer.HandlerIndex != TxCircleBuffer.TotalIndex)
		{
			Usart->Control1.TxEmptyInterruptEnable = true;
		}
	}
}
//------------------------------------------------------------------------------
void HermesConsoleTask(void* arg)
{
	 // just wait for other initialization messages to be emitted
	vTaskDelay(pdMS_TO_TICKS(1000));

	zprintf(MEDIUM_IMPORTANCE, "\r\nEnter 'help' for help\r\n");

	while(true)
	{
		zprintf(CRITICAL_IMPORTANCE, "HERMES >");
		HermesConsoleReadLine((uint8_t *)consol_input, sizeof(consol_input));
		zprintf(CRITICAL_IMPORTANCE, "\r\n");

		BaseType_t result;
		do
		{
			// keep looping around until all output has been generated

			// process input and generate some output
			result = FreeRTOS_CLIProcessCommand(consol_input, consol_output, sizeof(consol_output));

			if(strlen(consol_output) > 256)
			{
				zprintf(CRITICAL_IMPORTANCE,"CLI Output too long! - system may become unstable");

				// we still want to know what the output was for debugging purposes
				consol_output[sizeof(consol_output) - 1] = '\0';
			}

			// print the partial output
			zprintf(CRITICAL_IMPORTANCE, consol_output);

			HermesConsoleFlush();
		}
		while(result == pdTRUE);

		osDelay(100);
	}
}
//==============================================================================
//initialization:

void HermesConsoleInit()
{
	xCircleBufferInit(&TxCircleBuffer, &huart3, hermes_console_tx_buffer, HERMES_CONSOL_TX_BUFFER_SIZE_MASK);
	xCircleBufferInit(&RxCircleBuffer, &huart3, hermes_console_rx_buffer, HERMES_CONSOL_RX_BUFFER_SIZE_MASK);

	Usart = (REG_UART_T*)huart3.Instance;

	Usart->Control1.ReceiverEnable = true;
	Usart->Control3.RxDMA_Enable = true;

	HAL_DMA_Start(&hdma_usart3_rx,
					(uint32_t)&Usart->RxData,
					(uint32_t)hermes_console_rx_buffer,
					sizeof(hermes_console_rx_buffer));

	hermes_consol_tx_semaphore = xSemaphoreCreateBinary();
	hermes_consol_write_mutex = xSemaphoreCreateMutex();
	hermes_consol_read_mutex = xSemaphoreCreateMutex();

	hermes_consol_tx_task_handle =
			xTaskCreateStatic(HermesConsoleTxTask, // Function that implements the task.
								"Console Tx Task", // Text name for the task.
								HERMES_CONSOL_TX_TASK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								HERMES_CONSOL_TX_TASK_PRIORITY, // Priority at which the task is created.
								hermes_consol_tx_task_stack, // Array to use as the task's stack.
								&hermes_consol_tx_task_object);
/*
	hermes_consol_task_handle =
			xTaskCreateStatic(HermesConsoleTask, // Function that implements the task.
								"Console Task", // Text name for the task.
								HERMES_CONSOL_TASK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								HERMES_CONSOL_TASK_PRIORITY, // Priority at which the task is created.
								hermes_consol_task_stack, // Array to use as the task's stack.
								&hermes_consol_task_object);*/

}
//==============================================================================
