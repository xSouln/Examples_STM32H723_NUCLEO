//==============================================================================
#ifndef X_TX_CONVERTER_H
#define X_TX_CONVERTER_H
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//==============================================================================
//includes:

#include "xTypes.h"
//==============================================================================
//defines:

//==============================================================================
//types:

//==============================================================================
//functions:

static uint64_t xConverterStrToUInt64(void* in)
{
	uint64_t value = 0;
	uint8_t* ptr = in;
	
	while (ptr)
	{
		uint8_t ch = *ptr;

		if (ch < 48 || ch > 57)
		{
			return value;
		}

		value *= 10;
		value += ch - 48;
		ptr++;
	}

	return value;
}
//------------------------------------------------------------------------------

static int xConverterUInt64ToStr(void* out, uint64_t value)
{
	uint8_t* ptr = out;
	uint8_t len = 0;

	if(!out)
	{
		return len;
	}

	while (value)
	{
		uint8_t ch = value % 10;

		value /= 10;

		ptr[len] = ch + 48;
		len++;
	}

	if (len == 1)
	{
		return len;
	}

	for (uint8_t i = 0; i < len / 2; i++)
	{
		uint8_t ch = ptr[i];

		ptr[i] = ptr[len - i - 1];
		ptr[len - i - 1] = ch;
	}

	return len;
}
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //X_TX_CONVERTER_H
