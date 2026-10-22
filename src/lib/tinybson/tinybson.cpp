/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file tinybson.cpp
 *
 * A simple subset SAX-style BSON parser and generator.
 */

#include <px4_platform_common/posix.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <systemlib/err.h>

#include "tinybson.h"

#如果 0
# define debug(fmt, args...)		do { PX4_INFO("BSON: " fmt, ##args); } 当(0)
#否则
# define debug(fmt, args...)		do { } 当(0)
#endif

#define CODER_CHECK(_c)		do { 如果 ((_c)->dead) { PX4_ERR("coder dead"); 返回 -1; }} 当(0)
#define CODER_KILL(_c, _reason)	do { PX4_ERR("killed: %s", _reason); (_c)->dead = true; 返回 -1; } 当(0)

static 整数
read_x(bson_decoder_t decoder, void *p, size_t s)
{
	CODER_CHECK(decoder);

	如果 (decoder->fd > -1) {
		整数 ret = ::read(decoder->fd, p, s);

		如果 (ret == s) {
			decoder->total_decoded_size += ret;
			返回 0;
		}

		返回 -1;
	}

	如果 (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		如果 (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		如果 ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		返回 0;
	}

	debug("no source");
	返回 -1;
}

static 整数
read_int8(bson_decoder_t decoder, int8_t *b)
{
	返回 read_x(decoder, b, sizeof(*b));
}

static 整数
read_int32(bson_decoder_t decoder, int32_t *i)
{
	返回 read_x(decoder, i, sizeof(*i));
}

static 整数
read_int64(bson_decoder_t decoder, int64_t *i)
{
	返回 read_x(decoder, i, sizeof(*i));
}

static 整数
read_double(bson_decoder_t decoder, double *d)
{
	返回 read_x(decoder, d, sizeof(*d));
}

整数
bson_decoder_init_file(bson_decoder_t decoder, 整数 fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->node.type = BSON_UNDEFINED;

	// read document size
	如果 (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	如果 (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	返回 0;
}

整数
bson_decoder_init_buf(bson_decoder_t decoder, void *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	如果 ((buf == nullptr) || (callback == nullptr)) {
		返回 -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = false;

	如果 (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} 否则 {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->node.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	如果 (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	如果 (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	如果 ((decoder->total_document_size > 0) && (decoder->total_document_size > (整数)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	返回 0;
}

整数
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	如果 (decoder->node.type == BSON_EOO) {
		如果 (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		如果 (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			返回 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	当 (decoder->pending > 0) {
		如果 (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	如果 (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->node.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->node.type);

	/* EOO is special; it has no name/data following */
	如果 (decoder->node.type == BSON_EOO) {
		decoder->node.name[0] = '\0';

	} 否则 如果 ((整数)decoder->node.type == 0xff) { // indicates erased FLASH
		decoder->dead = true;
		返回 -ENODATA;

	} 否则 {

		/* get the node name */
		nlen = 0;

		为 (;;) {
			如果 (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->node.type, decoder->node.name);
				CODER_KILL(decoder, "node name overflow");
			}

			如果 (read_int8(decoder, (int8_t *)&decoder->node.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			如果 (decoder->node.name[nlen] == '\0') {
				break;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->node.name);

		switch (decoder->node.type) {
		case BSON_DOUBLE:
			如果 (read_double(decoder, &decoder->node.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->count_node_double++;
			break;

		case BSON_STRING:
			如果 (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->count_node_string++;
			break;

		case BSON_BINDATA:
			如果 (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			如果 (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->node.subtype = (bson_binary_subtype_t)tbyte;
			decoder->count_node_bindata++;
			break;

		case BSON_BOOL:
			如果 (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->node.b = (tbyte != 0);
			decoder->count_node_bool++;
			break;

		case BSON_INT32:
			如果 (read_int32(decoder, &decoder->node.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int32++;
			break;

		case BSON_INT64:
			如果 (read_int64(decoder, &decoder->node.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int64++;
			break;

		case BSON_nullptr:
		case BSON_UNDEFINED:
			break;

		/* XXX currently not supporting other types */
		default:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	返回 decoder->callback(decoder, &decoder->node);
}

整数
bson_decoder_copy_data(bson_decoder_t decoder, void *buf)
{
	整数 result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	如果 (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	返回 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	返回 decoder->pending;
}

static 整数
write_x(bson_encoder_t encoder, const void *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	如果 (encoder->fd > -1 && encoder->buf == nullptr) {
		整数 ret = ::write(encoder->fd, p, s);

		如果 (ret == s) {
			encoder->total_document_size += ret;
			返回 0;
		}

		返回 -1;
	}

	/* do we need to extend the buffer? */
	当 ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		如果 (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			整数 ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

			如果 (ret == (整数)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				如果 ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				break;

			} 否则 {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		如果 (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		如果 (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	返回 0;
}

static 整数
write_int8(bson_encoder_t encoder, int8_t b)
{
	返回 write_x(encoder, &b, sizeof(b));
}

static 整数
write_int32(bson_encoder_t encoder, int32_t i)
{
	返回 write_x(encoder, &i, sizeof(i));
}

static 整数
write_int64(bson_encoder_t encoder, int64_t i)
{
	返回 write_x(encoder, &i, sizeof(i));
}

static 整数
write_double(bson_encoder_t encoder, double d)
{
	返回 write_x(encoder, &d, sizeof(d));
}

static 整数
write_name(bson_encoder_t encoder, const char *name)
{
	size_t len = strlen(name);

	如果 (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	返回 write_x(encoder, name, len + 1);
}

整数
bson_encoder_init_file(bson_encoder_t encoder, 整数 fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = false;
	encoder->total_document_size = 0;

	如果 (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	返回 0;
}

整数
bson_encoder_init_buf_file(bson_encoder_t encoder, 整数 fd, void *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = false;
	encoder->realloc_ok = false;
	encoder->total_document_size = 0;

	如果 (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	返回 0;
}

整数
bson_encoder_init_buf(bson_encoder_t encoder, void *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	如果 (encoder->buf == nullptr) {
		encoder->realloc_ok = true;

	} 否则 {
		encoder->bufsize = bufsize;
	}

	如果 (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	返回 0;
}

整数
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	如果 (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	如果 (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		整数 ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

		如果 (ret == (整数)encoder->bufpos) {
			encoder->total_document_size += ret;

		} 否则 {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	const int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	如果 (encoder->fd > -1) {
		如果 ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::write(encoder->fd, &bson_doc_bytes, sizeof(bson_doc_bytes)) != sizeof(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} 否则 如果 (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, sizeof(bson_doc_bytes));
	}

	返回 0;
}

整数
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	如果 (encoder->fd > -1) {
		返回 -1;
	}

	返回 encoder->bufpos;
}

void *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	如果 (encoder->fd > -1) {
		返回 nullptr;
	}

	返回 encoder->buf;
}

整数 bson_encoder_append_bool(bson_encoder_t encoder, const char *name, bool value)
{
	CODER_CHECK(encoder);

	如果 (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	返回 0;
}

整数
bson_encoder_append_int32(bson_encoder_t encoder, const char *name, int32_t value)
{
	CODER_CHECK(encoder);

	如果 (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	返回 0;
}

整数
bson_encoder_append_int64(bson_encoder_t encoder, const char *name, int64_t value)
{
	CODER_CHECK(encoder);

	如果 (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	返回 0;
}

整数
bson_encoder_append_double(bson_encoder_t encoder, const char *name, double value)
{
	CODER_CHECK(encoder);

	如果 (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	返回 0;
}

整数
bson_encoder_append_string(bson_encoder_t encoder, const char *name, const char *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	如果 (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	返回 0;
}

整数
bson_encoder_append_binary(bson_encoder_t encoder, const char *name, bson_binary_subtype_t subtype, size_t size,
			   const void *data)
{
	CODER_CHECK(encoder);

	如果 (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	返回 0;
}

整数
bson_encoder_append_null(bson_encoder_t encoder, const char *name)
{
	CODER_CHECK(encoder);

	如果 (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	返回 0;
}
