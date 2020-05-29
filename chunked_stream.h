/*
 * chunked_stream.h
 *
 *  Created on: Sep 15, 2013
 *      Author: jimg
 */

#ifndef CHUNK_STREAM_H_
#define CHUNK_STREAM_H_

// These are the three chunk types
#define CHUNK_DATA 0x00000000
#define CHUNK_END  0x01000000
#define CHUNK_ERR  0x02000000

// This is the bit in the chunk that indicates the byte-order of the data,
// not the byte order of the chunk. The chunk is always in network byte order.
#define CHUNK_LITTLE_ENDIAN  0x04000000

// Chunk type mask masks off the low bytes and the little endian bit.
// The three chunk types (DATA, END and ERR) are mutually exclusive.
#define CHUNK_TYPE_MASK 0x03000000
#define CHUNK_SIZE_MASK 0x00FFFFFF

#define CHUNK_SIZE 4096

#define BYTE_ORDER_PREFIX 0
#define HEADER_IN_NETWORK_BYTE_ORDER 1

#endif /* CHUNK_STREAM_H_ */
