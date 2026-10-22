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

#ако 0
# define debug(fmt, args...)		do { PX4_INFO("BSON: " fmt, ##args); } док(0)
#инако
# define debug(fmt, args...)		do { } док(0)
#endif

#define CODER_CHECK(_c)		do { ако ((_c)->dead) { PX4_ERR("coder dead"); врати -1; }} док(0)
#define CODER_KILL(_c, _reason)	do { PX4_ERR("killed: %s", _reason); (_c)->dead = true; врати -1; } док(0)

static цео
read_x(bson_decoder_t decoder, void *p, size_t s)
{
	CODER_CHECK(decoder);

	ако (decoder->fd > -1) {
		цео ret = ::read(decoder->fd, p, s);

		ако (ret == s) {
			decoder->total_decoded_size += ret;
			врати 0;
		}

		врати -1;
	}

	ако (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		ако (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ако ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		врати 0;
	}

	debug("no source");
	врати -1;
}

static цео
read_int8(bson_decoder_t decoder, int8_t *b)
{
	врати read_x(decoder, b, sizeof(*b));
}

static цео
read_int32(bson_decoder_t decoder, int32_t *i)
{
	врати read_x(decoder, i, sizeof(*i));
}

static цео
read_int64(bson_decoder_t decoder, int64_t *i)
{
	врати read_x(decoder, i, sizeof(*i));
}

static цео
read_double(bson_decoder_t decoder, double *d)
{
	врати read_x(decoder, d, sizeof(*d));
}

цео
bson_decoder_init_file(bson_decoder_t decoder, цео fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->node.type = BSON_UNDEFINED;

	// read document size
	ако (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	ако (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	врати 0;
}

цео
bson_decoder_init_buf(bson_decoder_t decoder, void *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ако ((buf == nullptr) || (callback == nullptr)) {
		врати -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = false;

	ако (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} инако {
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

	ако (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ако (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ако ((decoder->total_document_size > 0) && (decoder->total_document_size > (цео)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	врати 0;
}

цео
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ако (decoder->node.type == BSON_EOO) {
		ако (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ако (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			врати 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	док (decoder->pending > 0) {
		ако (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ако (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->node.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->node.type);

	/* EOO is special; it has no name/data following */
	ако (decoder->node.type == BSON_EOO) {
		decoder->node.name[0] = '\0';

	} инако ако ((цео)decoder->node.type == 0xff) { // indicates erased FLASH
		decoder->dead = true;
		врати -ENODATA;

	} инако {

		/* get the node name */
		nlen = 0;

		за (;;) {
			ако (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->node.type, decoder->node.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ако (read_int8(decoder, (int8_t *)&decoder->node.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ако (decoder->node.name[nlen] == '\0') {
				break;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->node.name);

		switch (decoder->node.type) {
		case BSON_DOUBLE:
			ако (read_double(decoder, &decoder->node.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->count_node_double++;
			break;

		case BSON_STRING:
			ако (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->count_node_string++;
			break;

		case BSON_BINDATA:
			ако (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ако (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->node.subtype = (bson_binary_subtype_t)tbyte;
			decoder->count_node_bindata++;
			break;

		case BSON_BOOL:
			ако (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->node.b = (tbyte != 0);
			decoder->count_node_bool++;
			break;

		case BSON_INT32:
			ако (read_int32(decoder, &decoder->node.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int32++;
			break;

		case BSON_INT64:
			ако (read_int64(decoder, &decoder->node.i64)) {
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
	врати decoder->callback(decoder, &decoder->node);
}

цео
bson_decoder_copy_data(bson_decoder_t decoder, void *buf)
{
	цео result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	ако (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	врати 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	врати decoder->pending;
}

static цео
write_x(bson_encoder_t encoder, const void *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ако (encoder->fd > -1 && encoder->buf == nullptr) {
		цео ret = ::write(encoder->fd, p, s);

		ако (ret == s) {
			encoder->total_document_size += ret;
			врати 0;
		}

		врати -1;
	}

	/* do we need to extend the buffer? */
	док ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ако (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			цео ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

			ако (ret == (цео)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				ако ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				break;

			} инако {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ако (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ако (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	врати 0;
}

static цео
write_int8(bson_encoder_t encoder, int8_t b)
{
	врати write_x(encoder, &b, sizeof(b));
}

static цео
write_int32(bson_encoder_t encoder, int32_t i)
{
	врати write_x(encoder, &i, sizeof(i));
}

static цео
write_int64(bson_encoder_t encoder, int64_t i)
{
	врати write_x(encoder, &i, sizeof(i));
}

static цео
write_double(bson_encoder_t encoder, double d)
{
	врати write_x(encoder, &d, sizeof(d));
}

static цео
write_name(bson_encoder_t encoder, const char *name)
{
	size_t len = strlen(name);

	ако (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	врати write_x(encoder, name, len + 1);
}

цео
bson_encoder_init_file(bson_encoder_t encoder, цео fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = false;
	encoder->total_document_size = 0;

	ако (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	врати 0;
}

цео
bson_encoder_init_buf_file(bson_encoder_t encoder, цео fd, void *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = false;
	encoder->realloc_ok = false;
	encoder->total_document_size = 0;

	ако (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	врати 0;
}

цео
bson_encoder_init_buf(bson_encoder_t encoder, void *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ако (encoder->buf == nullptr) {
		encoder->realloc_ok = true;

	} инако {
		encoder->bufsize = bufsize;
	}

	ако (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	врати 0;
}

цео
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ако (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ако (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		цео ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

		ако (ret == (цео)encoder->bufpos) {
			encoder->total_document_size += ret;

		} инако {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	const int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	ако (encoder->fd > -1) {
		ако ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::write(encoder->fd, &bson_doc_bytes, sizeof(bson_doc_bytes)) != sizeof(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} инако ако (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, sizeof(bson_doc_bytes));
	}

	врати 0;
}

цео
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ако (encoder->fd > -1) {
		врати -1;
	}

	врати encoder->bufpos;
}

void *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ако (encoder->fd > -1) {
		врати nullptr;
	}

	врати encoder->buf;
}

цео bson_encoder_append_bool(bson_encoder_t encoder, const char *name, bool value)
{
	CODER_CHECK(encoder);

	ако (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	врати 0;
}

цео
bson_encoder_append_int32(bson_encoder_t encoder, const char *name, int32_t value)
{
	CODER_CHECK(encoder);

	ако (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	врати 0;
}

цео
bson_encoder_append_int64(bson_encoder_t encoder, const char *name, int64_t value)
{
	CODER_CHECK(encoder);

	ако (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	врати 0;
}

цео
bson_encoder_append_double(bson_encoder_t encoder, const char *name, double value)
{
	CODER_CHECK(encoder);

	ако (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	врати 0;
}

цео
bson_encoder_append_string(bson_encoder_t encoder, const char *name, const char *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	ако (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	врати 0;
}

цео
bson_encoder_append_binary(bson_encoder_t encoder, const char *name, bson_binary_subtype_t subtype, size_t size,
			   const void *data)
{
	CODER_CHECK(encoder);

	ако (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	врати 0;
}

цео
bson_encoder_append_null(bson_encoder_t encoder, const char *name)
{
	CODER_CHECK(encoder);

	ако (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	врати 0;
}
