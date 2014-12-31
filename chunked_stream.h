/*
 * chunked_stream.h
 *
 *  Created on: Sep 15, 2013
 *      Author: jimg
 */

#ifndef CHUNK_STREAM_H_
#define CHUNK_STREAM_H_

#define CHUNK_DATA 0x00000000
#define CHUNK_END  0x01000000
#define CHUNK_ERR  0x02000000

#if !BYTE_ORDER_PREFIX
// LITTLE or BIG endian if set? jhrg 11/26/13
// #define CHUNK_BIG_ENDIAN  0x04000000

#define CHUNK_LITTLE_ENDIAN  0x04000000
#endif

// Chunk type mask masks off the low bytes and the little endian bit.
// The three chunk types (DATA, END and ERR) are mutually exclusive.
#define CHUNK_TYPE_MASK 0x03000000
#define CHUNK_SIZE_MASK 0x00FFFFFF

#define CHUNK_SIZE 4096

#endif /* CHUNK_STREAM_H_ */
