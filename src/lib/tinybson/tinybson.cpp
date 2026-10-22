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

#ถ้า 0
# define debug(fmt, args...)		do { PX4_INFO("BSON: " fmt, ##args); } ขณะ(0)
#มิฉะนั้น
# define debug(fmt, args...)		do { } ขณะ(0)
#endif

#define CODER_CHECK(_c)		do { ถ้า ((_c)->dead) { PX4_ERR("coder dead"); คืน -1; }} ขณะ(0)
#define CODER_KILL(_c, _reason)	do { PX4_ERR("killed: %s", _reason); (_c)->dead = true; คืน -1; } ขณะ(0)

static จำนวนเต็ม
read_x(bson_decoder_t decoder, void *p, size_t s)
{
	CODER_CHECK(decoder);

	ถ้า (decoder->fd > -1) {
		จำนวนเต็ม ret = ::read(decoder->fd, p, s);

		ถ้า (ret == s) {
			decoder->total_decoded_size += ret;
			คืน 0;
		}

		คืน -1;
	}

	ถ้า (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		ถ้า (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ถ้า ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		คืน 0;
	}

	debug("no source");
	คืน -1;
}

static จำนวนเต็ม
read_int8(bson_decoder_t decoder, int8_t *b)
{
	คืน read_x(decoder, b, parimaa_na(*b));
}

static จำนวนเต็ม
read_int32(bson_decoder_t decoder, int32_t *i)
{
	คืน read_x(decoder, i, parimaa_na(*i));
}

static จำนวนเต็ม
read_int64(bson_decoder_t decoder, int64_t *i)
{
	คืน read_x(decoder, i, parimaa_na(*i));
}

static จำนวนเต็ม
read_double(bson_decoder_t decoder, dviguu_na *d)
{
	คืน read_x(decoder, d, parimaa_na(*d));
}

จำนวนเต็ม
bson_decoder_init_file(bson_decoder_t decoder, จำนวนเต็ม fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->node.type = BSON_UNDEFINED;

	// read document size
	ถ้า (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	ถ้า (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	คืน 0;
}

จำนวนเต็ม
bson_decoder_init_buf(bson_decoder_t decoder, void *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ถ้า ((buf == nullptr) || (callback == nullptr)) {
		คืน -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = false;

	ถ้า (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} มิฉะนั้น {
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

	ถ้า (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ถ้า (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ถ้า ((decoder->total_document_size > 0) && (decoder->total_document_size > (จำนวนเต็ม)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	คืน 0;
}

จำนวนเต็ม
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ถ้า (decoder->node.type == BSON_EOO) {
		ถ้า (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ถ้า (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			คืน 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	ขณะ (decoder->pending > 0) {
		ถ้า (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ถ้า (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->node.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->node.type);

	/* EOO is special; it has no name/data following */
	ถ้า (decoder->node.type == BSON_EOO) {
		decoder->node.name[0] = '\0';

	} มิฉะนั้น ถ้า ((จำนวนเต็ม)decoder->node.type == 0xff) { // indicates erased FLASH
		decoder->dead = true;
		คืน -ENODATA;

	} มิฉะนั้น {

		/* get the node name */
		nlen = 0;

		สำหรับ (;;) {
			ถ้า (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->node.type, decoder->node.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ถ้า (read_int8(decoder, (int8_t *)&decoder->node.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ถ้า (decoder->node.name[nlen] == '\0') {
				bha_nga;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->node.name);

		switch (decoder->node.type) {
		pak_sa BSON_DOUBLE:
			ถ้า (read_double(decoder, &decoder->node.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->count_node_double++;
			bha_nga;

		pak_sa BSON_STRING:
			ถ้า (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->count_node_string++;
			bha_nga;

		pak_sa BSON_BINDATA:
			ถ้า (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ถ้า (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->node.subtype = (bson_binary_subtype_t)tbyte;
			decoder->count_node_bindata++;
			bha_nga;

		pak_sa BSON_BOOL:
			ถ้า (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->node.b = (tbyte != 0);
			decoder->count_node_bool++;
			bha_nga;

		pak_sa BSON_INT32:
			ถ้า (read_int32(decoder, &decoder->node.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int32++;
			bha_nga;

		pak_sa BSON_INT64:
			ถ้า (read_int64(decoder, &decoder->node.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int64++;
			bha_nga;

		pak_sa BSON_nullptr:
		pak_sa BSON_UNDEFINED:
			bha_nga;

		/* XXX currently not supporting other types */
		default:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	คืน decoder->callback(decoder, &decoder->node);
}

จำนวนเต็ม
bson_decoder_copy_data(bson_decoder_t decoder, void *buf)
{
	จำนวนเต็ม result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	ถ้า (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	คืน 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	คืน decoder->pending;
}

static จำนวนเต็ม
write_x(bson_encoder_t encoder, const void *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ถ้า (encoder->fd > -1 && encoder->buf == nullptr) {
		จำนวนเต็ม ret = ::write(encoder->fd, p, s);

		ถ้า (ret == s) {
			encoder->total_document_size += ret;
			คืน 0;
		}

		คืน -1;
	}

	/* do we need to extend the buffer? */
	ขณะ ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ถ้า (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			จำนวนเต็ม ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

			ถ้า (ret == (จำนวนเต็ม)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				ถ้า ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				bha_nga;

			} มิฉะนั้น {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ถ้า (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ถ้า (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	คืน 0;
}

static จำนวนเต็ม
write_int8(bson_encoder_t encoder, int8_t b)
{
	คืน write_x(encoder, &b, parimaa_na(b));
}

static จำนวนเต็ม
write_int32(bson_encoder_t encoder, int32_t i)
{
	คืน write_x(encoder, &i, parimaa_na(i));
}

static จำนวนเต็ม
write_int64(bson_encoder_t encoder, int64_t i)
{
	คืน write_x(encoder, &i, parimaa_na(i));
}

static จำนวนเต็ม
write_double(bson_encoder_t encoder, dviguu_na d)
{
	คืน write_x(encoder, &d, parimaa_na(d));
}

static จำนวนเต็ม
write_name(bson_encoder_t encoder, const var_na *name)
{
	size_t len = strlen(name);

	ถ้า (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	คืน write_x(encoder, name, len + 1);
}

จำนวนเต็ม
bson_encoder_init_file(bson_encoder_t encoder, จำนวนเต็ม fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = false;
	encoder->total_document_size = 0;

	ถ้า (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_init_buf_file(bson_encoder_t encoder, จำนวนเต็ม fd, void *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = false;
	encoder->realloc_ok = false;
	encoder->total_document_size = 0;

	ถ้า (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_init_buf(bson_encoder_t encoder, void *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ถ้า (encoder->buf == nullptr) {
		encoder->realloc_ok = true;

	} มิฉะนั้น {
		encoder->bufsize = bufsize;
	}

	ถ้า (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ถ้า (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ถ้า (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		จำนวนเต็ม ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

		ถ้า (ret == (จำนวนเต็ม)encoder->bufpos) {
			encoder->total_document_size += ret;

		} มิฉะนั้น {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	const int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	ถ้า (encoder->fd > -1) {
		ถ้า ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::write(encoder->fd, &bson_doc_bytes, parimaa_na(bson_doc_bytes)) != parimaa_na(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} มิฉะนั้น ถ้า (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, parimaa_na(bson_doc_bytes));
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ถ้า (encoder->fd > -1) {
		คืน -1;
	}

	คืน encoder->bufpos;
}

void *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ถ้า (encoder->fd > -1) {
		คืน nullptr;
	}

	คืน encoder->buf;
}

จำนวนเต็ม bson_encoder_append_bool(bson_encoder_t encoder, const var_na *name, bool value)
{
	CODER_CHECK(encoder);

	ถ้า (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_append_int32(bson_encoder_t encoder, const var_na *name, int32_t value)
{
	CODER_CHECK(encoder);

	ถ้า (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_append_int64(bson_encoder_t encoder, const var_na *name, int64_t value)
{
	CODER_CHECK(encoder);

	ถ้า (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_append_double(bson_encoder_t encoder, const var_na *name, dviguu_na value)
{
	CODER_CHECK(encoder);

	ถ้า (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_append_string(bson_encoder_t encoder, const var_na *name, const var_na *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	ถ้า (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_append_binary(bson_encoder_t encoder, const var_na *name, bson_binary_subtype_t subtype, size_t size,
			   const void *data)
{
	CODER_CHECK(encoder);

	ถ้า (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	คืน 0;
}

จำนวนเต็ม
bson_encoder_append_null(bson_encoder_t encoder, const var_na *name)
{
	CODER_CHECK(encoder);

	ถ้า (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	คืน 0;
}
