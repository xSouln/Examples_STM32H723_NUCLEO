/*
 * xType.h
 *
 * Created: 16.05.2019 14:29:38
 *  Author: souln
 */ 
//==============================================================================
#ifndef X_TYPES_H
#define X_TYPES_H
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//==============================================================================
//includes:

#include <stdint.h>
#include <stdbool.h>
//==============================================================================
//types:

typedef enum
{
	xResultAccept = 0U,
	xResultError,
	xResultInvalidParameter,
	xResultBusy,
	xResultTimeOut,
	xResultNotSupported,
	xResultValueIsNotFound,
	xResultRequestIsNotFound,
	xResultLinkError,
	xResulComponentInitializationError,
	xResultOutOfRange,
	xResultNotFound,
	
} xResult;
//------------------------------------------------------------------------------

typedef void* xObject;
//==============================================================================

#define OBJECT_BASE\
	const void* Description;\
	parent_type* Parent
//------------------------------------------------------------------------------

typedef struct _ObjectBaseT
{
	const void* Description;
	void* Parent;

} ObjectBaseT;
//------------------------------------------------------------------------------

#define OBJECT_BASE_T(heder_name, patent_type)\
struct\
{\
	OBJECT_BASE(patent_type);\
} heder_name
//------------------------------------------------------------------------------

#define OBJECT_DESCRIPTION_KEY 0xFE0000EF
typedef struct
{
	uint32_t Key;
	uint32_t ObjectId;
	char* Type;

} ObjectDescriptionT;
//==============================================================================

#define DEFINITION_EVENT_TYPE(name, selector_type, arg_type)\
typedef struct x##name##EventStruct\
{\
	ObjectDescriptionT* object;\
	union\
	{\
		struct\
		{\
			selector_type Selector;\
		};\
		uint32_t SelectorValue;\
	};\
	\
	arg_type Arg;\
	struct x##name##EventStruct* Primary;\
} x##name##EventT
//------------------------------------------------------------------------------

DEFINITION_EVENT_TYPE( ,uint32_t, void*);
//==============================================================================

typedef enum
{
	PropertyGet,
	PropertySet,

} PropertyActions;
//------------------------------------------------------------------------------

#define nameof(value) #value
//------------------------------------------------------------------------------
#define DEFINITION_HANDLER_TYPE(name)\
typedef void (*name##HandlerT)(void* object)

#define DEFINITION_IRQ_LISTENER_TYPE(name)\
typedef void (*name##IRQListenerT)(void* object)

#define DEFINITION_EVENT_LISTENER_TYPE(name, selector_type)\
typedef void (*name##EventListenerT)(void* object, selector_type selector, void* arg, ...)

#define DEFINITION_REQUEST_LISTENER_TYPE(name, selector_type)\
typedef xResult (*name##RequestListenerT)(void* object, selector_type selector, void* arg, ...)

#define DEFINITION_GET_VALUE_ACTION_TYPE(name, selector_type)\
typedef xResult (*name##ActionT)(void* object, selector_type selector, void* arg)

#define DEFINITION_SET_VALUE_ACTION_TYPE(name, selector_type)\
typedef xResult (*name##ActionT)(void* object, selector_type selector, void* arg)

#define DEFINITION_PROPERTY_TYPE(name, value_type)\
typedef xResult (*name##PropertyT)(void* object, PropertyActions action, value_type* value)
//------------------------------------------------------------------------------
/*
#define DEFINITION_HANDLER_PRIVATE_FUNCTION(name)\
static void Private##name##Handler(void* object)

#define DEFINITION_IRQ_LISTENER_PRIVATE_FUNCTION(name)\
static void Private##name##IRQListener(void* object)

#define DEFINITION_EVENT_LISTENER_PRIVATE_FUNCTION(name)\
static void Private##name##EventListener(void* object, selector_type selector, void* arg, ...)

#define DEFINITION_REQUEST_LISTENER_PRIVATE_FUNCTION(name)\
static void Private##name##REQUESTListener(void* object, selector_type selector, void* arg, ...)

#define DEFINITION_HANDLER_PRIVATE_FUNCTION(name)\
static void Private##name##Handler(void* object)

#define DEFINITION_HANDLER_PRIVATE_FUNCTION(name)\
static void Private##name##Handler(void* object)
*/
//------------------------------------------------------------------------------

#define DECLARE_HANDLER(name)\
name##HandlerT Handler

#define DECLARE_IRQ_LISTENER(name)\
name##IRQListenerT IRQListener

#define DECLARE_EVENT_LISTENER(name)\
name##EventListenerT EventListener

#define DECLARE_REQUEST_LISTENER(name)\
name##RequestListenerT RequestListener

#define DECLARE_GET_VALUE_ACTION(name)\
name##ActionT GetValue

#define DECLARE_SET_VALUE_ACTION(name)\
name##ActionT SetValue

#define DECLARE_PROPERTY(name, value_name)\
name##PropertyT value_name
//------------------------------------------------------------------------------

#define INITIALIZATION_HANDLER(name, function)\
.Handler = (name##HandlerT)function

#define INITIALIZATION_EVENT_LISTENER(name, function)\
.EventListener = (name##EventListenerT)function

#define INITIALIZATION_IRQ_LISTENER(name, function)\
.IRQListener = (name##IRQListenerT)function

#define INITIALIZATION_REQUEST_LISTENER(name, function)\
.RequestListener = (name##RequestListenerT)function

#define INITIALIZATION_GET_VALUE_ACTION(name, function)\
.GetValue = (name##ActionT)function

#define INITIALIZATION_SET_VALUE_ACTION(name, function)\
.SetValue = (name##ActionT)function

#define INITIALIZATION_PROPERTY(name, value_name, function)\
.value_name = (name##PropertyT)function
//==============================================================================

#define SIZE_STRING(str)(sizeof(str) - 1)
#define SIZE_ARRAY(array)(sizeof(array) / sizeof(array[0]))

#define sizeof_str(str)(sizeof(str) / sizeof(str[0]) - 1)
#define sizeof_array(array)(sizeof(array) / sizeof(array[0]))

//max value: 4294967295 - 10 symbols
#define uitn32_t_maximum_length_in_string 10

//max value: 9223372036854775807 - 19 symbols
#define uitn64_t_maximum_length_in_string 20
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //X_TYPES_H
