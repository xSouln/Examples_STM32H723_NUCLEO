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
//------------------------------------------------------------------------------

static uint64_t xConverterSwapUInt64(uint64_t val)
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}
//------------------------------------------------------------------------------

static uint64_t xConverterStrHexToUInt64(char *hex, int len)
{
	uint64_t result = 0;
	bool start_decode = false;

    while (*hex && len)
    {
        // get current character then increment
        char byte = *hex++;
        len--;

        if (!start_decode
        && (byte == '0' || byte == 'x' || byte == 'X'))
		{
			continue;
		}

        start_decode = true;

        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
		{
        	byte = byte - '0';
		}
        else if (byte >= 'a' && byte <='f')
		{
        	byte = byte - 'a' + 10;
		}
        else if (byte >= 'A' && byte <='F')
		{
        	byte = byte - 'A' + 10;
		}

        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        result = (result << 4) | (byte & 0xF);
    }

    return result;
}
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //X_TX_CONVERTER_H
