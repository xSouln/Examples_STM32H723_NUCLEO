//==============================================================================
//includes:

#include "Components.h"

#include "Hermes/Sources/SNTP.h"
//==============================================================================
//variables:

REG_TIM_T* Timer4 = (REG_TIM_T*)TIM4;

#ifdef TIM2
REG_TIM_T* Timer2 = (REG_TIM_T*)TIM2;
#endif

#ifdef TIM3
REG_TIM_T* Timer3 = (REG_TIM_T*)TIM3;
#endif

#ifdef TIM12
REG_TIM_T* Timer12 = (REG_TIM_T*)TIM12;
#endif

static uint8_t time1_ms;
static uint8_t time5_ms;
static uint16_t time1000_ms;

static uint32_t led_toggle_time_stamp;
static uint32_t sntp_update_time_stamp;
//==============================================================================
//functions:
static inline uint32_t PrivateGetTimeDifference(uint32_t time_stamp)
{
	return 0;
}
//------------------------------------------------------------------------------

static void PrivateTerminalComponentEventListener(TerminalT* terminal, TerminalEventSelector selector, void* arg, ...)
{
	switch((int)selector)
	{
		case TerminalEventTime_1000ms:
			xTxTransferSetTxLine(&Terminal.Transfer, &SerialPortUART.Tx);
			xTxTransferStart(&Terminal.Transfer, "qwerty", 6);
			break;
		default: break;
	}
}
//------------------------------------------------------------------------------
#ifdef SERIAL_PORT_COMPONENT_ENABLE
static void PrivateSerialPortComponentEventListener(SerialPortT* port, SerialPortEventSelector selector, void* arg, ...)
{
	switch((int)selector)
	{
		case SerialPortEventEndLine:
			#ifdef TERMINAL_COMPONENT_ENABLE
			TerminalReceiveData(&port->Rx,
								((SerialPortReceivedDataT*)arg)->Data,
								((SerialPortReceivedDataT*)arg)->Size);
			#endif
			break;

		case SerialPortEventBufferIsFull:
			#ifdef TERMINAL_COMPONENT_ENABLE
			TerminalReceiveData(&port->Rx,
								((SerialPortReceivedDataT*)arg)->Data,
								((SerialPortReceivedDataT*)arg)->Size);
			#endif
			break;

		default: break;
	}
}
#endif
//==============================================================================
//default functions:

void ComponentsEventListener(ComponentObjectBaseT* object, int selector, void* arg, ...)
{
	if (object->Description->Key != OBJECT_DESCRIPTION_KEY)
	{
		return;
	}

	switch(object->Description->ObjectId)
	{
		#ifdef SERIAL_PORT_COMPONENT_ENABLE
		case SERIAL_PORT_OBJECT_ID:
			PrivateSerialPortComponentEventListener((SerialPortT*)object, selector, arg);
			break;
		#endif

		case TERMINAL_OBJECT_ID:
			PrivateTerminalComponentEventListener((TerminalT*)object, selector, arg);
			break;
	}
}
//------------------------------------------------------------------------------

void ComponentsRequestListener(ComponentObjectBaseT* port, int selector, void* arg, ...)
{
	switch((int)selector)
	{
		default: break;
	}
}
//------------------------------------------------------------------------------
/**
 * @brief main handler
 */
void ComponentsHandler()
{
	extern bool SNTP_GetTime(void);

	if (!time1_ms)
	{
		time1_ms = 1;
	}

	if (!time5_ms)
	{
		time5_ms = 5;

		/*
		#ifdef SERIAL_PORT_COMPONENT_ENABLE
		SerialPortComponentHandler();
		#endif

		#ifdef TERMINAL_COMPONENT_ENABLE
		TerminalComponentHandler();
		#endif

		#ifdef TCP_SERVER_COMPONENT_ENABLE
		TCPServerComponentHandler();
		#endif
		*/
	}

	#ifdef ZIGBEE_COMPONENT_ENABLE
	ZigbeeComponentHandler();
	#endif

	#ifdef SUREFLAP_COMPONENT_ENABLE
	SureFlapComponentHandler();
	#endif

	if (ComponentsSysGetTime() - led_toggle_time_stamp > 999)
	{
		//time1000_ms = 1000;

		led_toggle_time_stamp = ComponentsSysGetTime();

		LED_YELLOW_GPIO_Port->ODR ^= LED_YELLOW_Pin;
	}

	if (ComponentsSysGetTime() - sntp_update_time_stamp > 10000)
	{
		sntp_update_time_stamp = ComponentsSysGetTime();

		//SNTP_GetTime();
	}
}
//------------------------------------------------------------------------------
/**
 * @brief time synchronization of time-dependent processes
 */
void ComponentsTimeSynchronization()
{
	#ifdef SERIAL_PORT_COMPONENT_ENABLE
	SerialPortComponentTimeSynchronization();
	#endif

	#ifdef TERMINAL_COMPONENT_ENABLE
	TerminalComponentTimeSynchronization();
	#endif

	#ifdef TCP_SERVER_COMPONENT_ENABLE
	TCPServerComponentTimeSynchronization();
	#endif

	#ifdef ZIGBEE_COMPONENT_ENABLE
	ZigbeeComponentTimeSynchronization();
	#endif

	#ifdef SUREFLAP_COMPONENT_ENABLE
	SureFlapComponentTimeSynchronization();
	#endif

	time1_ms = 0;

	if (time5_ms)
	{
		time5_ms--;
	}

	if (time1000_ms)
	{
		time1000_ms--;
	}
}
//------------------------------------------------------------------------------

void ComponentsSystemDelay(ComponentObjectBaseT* object, uint32_t time)
{
	HAL_Delay(time);
}
//------------------------------------------------------------------------------

void ComponentsTrace(char* text)
{

}
//------------------------------------------------------------------------------

void ComponentsSystemEnableIRQ()
{

}
//------------------------------------------------------------------------------

void ComponentsSystemDisableIRQ()
{

}
//------------------------------------------------------------------------------

void ComponentsSystemReset()
{

}
//------------------------------------------------------------------------------

uint32_t ComponentsSystemGetTime()
{
	return HAL_GetTick();
}
//------------------------------------------------------------------------------
/**
 * @brief initializing the component
 * @param parent binding to the parent object
 * @return int
 */
xResult ComponentsInit(void* parent)
{
	#if SERIAL_PORT_COMPONENT_ENABLE
	SerialPortComponentInit(parent);
	#endif
/*
	#ifdef TCP_SERVER_COMPONENT_ENABLE
	TCPServerComponentInit(parent);
	#endif
*/
	#ifdef TERMINAL_COMPONENT_ENABLE
	TerminalComponentInit(parent);
	#endif

	#ifdef SUREFLAP_COMPONENT_ENABLE
	SureFlapComponentInit(parent);
	#endif

	#ifdef ZIGBEE_COMPONENT_ENABLE
	ZigbeeComponentInit(parent);
	#endif

	Timer4->DMAOrInterrupts.UpdateInterruptEnable = true;
	Timer4->Control1.CounterEnable = true;

	TerminalTxBind(&SerialPortUART.Tx);

	sntp_update_time_stamp = 20000;

	Timer2->Control1.CounterEnable = true;
	//Timer3->Control1.CounterEnable = true;
	//Timer12->Control1.CounterEnable = true;
	//SNTP_Init();

	return xResultAccept;
}
//==============================================================================
