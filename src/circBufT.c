// *******************************************************
//
// circBufT.c
//
// Support for a circular buffer of uint32_t values on the
//  Tiva processor.
// P.J. Bones UCECE
// Last modified:  8.3.2017
//
// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "circBufT.h"

static uint32_t circBuffer[BUF_SIZE];

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer. Return a pointer to circBuffer.
uint32_t *initCircBuf(circBuf_t *buffer, uint32_t size)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = circBuffer;
	return buffer->data;
}

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).
void writeCircBuf(circBuf_t *buffer, uint32_t entry)
{
	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
		buffer->windex = 0;
}

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function deos not check
// if reading has advanced ahead of writing.
uint32_t readCircBuf(circBuf_t *buffer)
{
	uint32_t entry;

	entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
		buffer->rindex = 0;
	return entry;
}

// **********************************************************
// averageCircBuf: returns the average of the contents of the
// circular buffer's contents
uint32_t averageCircBuf(circBuf_t *buffer)
{
    uint32_t sum = 0;

    // Accumulate samples
    for (uint8_t i = 0; i < buffer->size; i++)
    {
        sum += readCircBuf(&circBuffer + i*sizeof(buffer));
    }

    // Calculate mean and round value
    return (2 * sum + buffer->size) / 2 / buffer->size;
}