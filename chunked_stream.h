/*
 * chunked_stream.h
 *
 *  Created on: Sep 13, 2013
 *      Author: jimg
 */

#ifndef CHUNKED_STREAM_H_
#define CHUNKED_STREAM_H_

// These define the kind of chunk. Each chunk has a 32-bit header where the
// 3 LS Bytes are the size (so chunks are limited to 2^24 bytes in size) and
// the MSB is the kind of chunk. Use bit-wise or ...
#define CHUNK_DATA 0x00000000
#define CHUNK_END 0x01000000
#define CHUNK_ERR 0x02000000

#define CHUNK_TYPE_MASK 0xFF000000
#define CHUNK_SIZE_MASK 0x00FFFFFF

#endif /* CHUNKED_STREAM_H_ */
