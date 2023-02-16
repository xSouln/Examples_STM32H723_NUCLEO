//==============================================================================
//includes:

#include "Hermes-console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cmsis_os.h"

#include "Common/xCircleBuffer.h"
#include "usart.h"
//==============================================================================
//defines:

#define HERMES_CONSOL_TASK_SIZE (0x100)

#define HERMES_CONSOL_RX_BUFFER_SIZE_MASK 0x3ff
#define HERMES_CONSOL_RX_BUFFER_SIZE (HERMES_CONSOL_RX_BUFFER_SIZE_MASK + 1)
#define HERMES_CONSOL_RX_BUFFER_MEM_LOCATION

#define HERMES_CONSOL_TX_BUFFER_SIZE_MASK 0x3ff
#define HERMES_CONSOL_TX_BUFFER_SIZE (HERMES_CONSOL_TX_BUFFER_SIZE_MASK + 1)
#define HERMES_CONSOL_TX_BUFFER_MEM_LOCATION
//==============================================================================
//externs:

extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
//==============================================================================
//overrides:


//==============================================================================
//variables:

//------------------------------------------------------------------------------
TaskHandle_t hermes_consol_task_handle;
StaticTask_t hermes_consol_task_object __attribute__((section("._user_dtcmram_ram")));
StackType_t hermes_consol_task_stack[HERMES_CONSOL_TASK_SIZE] __attribute__((section("._user_dtcmram_ram")));

SemaphoreHandle_t hermes_consol_tx_semaphore;

SemaphoreHandle_t hermes_consol_write_mutex = 0;
SemaphoreHandle_t hermes_consol_read_mutex = 0;
//------------------------------------------------------------------------------
static uint8_t hermes_console_rx_buffer[HERMES_CONSOL_RX_BUFFER_SIZE] HERMES_CONSOL_RX_BUFFER_MEM_LOCATION;
static uint8_t hermes_console_tx_buffer[HERMES_CONSOL_TX_BUFFER_SIZE] HERMES_CONSOL_TX_BUFFER_MEM_LOCATION;

static xCircleBufferT RxCircleBuffer;
static xCircleBufferT TxCircleBuffer;

static REG_UART_T* Usart;
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
int HermesConsoleRead(uint8_t* data, uint16_t size, uint8_t mode)
{
	HERMES_CONSOL_READ_BLOCK();

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
int HermesConsoleWrite(void* data, uint16_t size, uint8_t mode)
{
	HERMES_CONSOL_WRITE_BLOCK();

	xCircleBufferAdd(&TxCircleBuffer, (uint8_t*)data, size);

	HERMES_CONSOL_WRITE_UNBLOCK();

	xSemaphoreGive(hermes_consol_tx_semaphore);

	return 0;
}
//------------------------------------------------------------------------------
void HermesConsoleReadLine()
{

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
void HermesConsoleTask(void* arg)
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

	hermes_consol_task_handle =
			xTaskCreateStatic(HermesConsoleTask, // Function that implements the task.
								"Console Task", // Text name for the task.
								HERMES_CONSOL_TASK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								osPriorityNormal, // Priority at which the task is created.
								hermes_consol_task_stack, // Array to use as the task's stack.
								&hermes_consol_task_object);

}
//==============================================================================
