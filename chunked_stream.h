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
#define CHUNK_TYPE_MASK 0xFF000000
#define CHUNK_SIZE_MASK 0x00FFFFFF

#endif /* CHUNK_STREAM_H_ */
