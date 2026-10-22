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

#အကယ်၍ 0
# define debug(fmt, args...)		do { PX4_INFO("BSON: " fmt, ##args); } နေစဉ်(0)
#မဟုတ်ပါက
# define debug(fmt, args...)		do { } နေစဉ်(0)
#endif

#define CODER_CHECK(_c)		do { အကယ်၍ ((_c)->dead) { PX4_ERR("coder dead"); ပြန်ပေး -1; }} နေစဉ်(0)
#define CODER_KILL(_c, _reason)	do { PX4_ERR("killed: %s", _reason); (_c)->dead = true; ပြန်ပေး -1; } နေစဉ်(0)

static ကိန်းပြည့်
read_x(bson_decoder_t decoder, void *p, size_t s)
{
	CODER_CHECK(decoder);

	အကယ်၍ (decoder->fd > -1) {
		ကိန်းပြည့် ret = ::read(decoder->fd, p, s);

		အကယ်၍ (ret == s) {
			decoder->total_decoded_size += ret;
			ပြန်ပေး 0;
		}

		ပြန်ပေး -1;
	}

	အကယ်၍ (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		အကယ်၍ (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		အကယ်၍ ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		ပြန်ပေး 0;
	}

	debug("no source");
	ပြန်ပေး -1;
}

static ကိန်းပြည့်
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ပြန်ပေး read_x(decoder, b, parimaa_na(*b));
}

static ကိန်းပြည့်
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ပြန်ပေး read_x(decoder, i, parimaa_na(*i));
}

static ကိန်းပြည့်
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ပြန်ပေး read_x(decoder, i, parimaa_na(*i));
}

static ကိန်းပြည့်
read_double(bson_decoder_t decoder, dviguu_na *d)
{
	ပြန်ပေး read_x(decoder, d, parimaa_na(*d));
}

ကိန်းပြည့်
bson_decoder_init_file(bson_decoder_t decoder, ကိန်းပြည့် fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->node.type = BSON_UNDEFINED;

	// read document size
	အကယ်၍ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	အကယ်၍ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_decoder_init_buf(bson_decoder_t decoder, void *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	အကယ်၍ ((buf == nullptr) || (callback == nullptr)) {
		ပြန်ပေး -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = false;

	အကယ်၍ (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} မဟုတ်ပါက {
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

	အကယ်၍ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	အကယ်၍ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	အကယ်၍ ((decoder->total_document_size > 0) && (decoder->total_document_size > (ကိန်းပြည့်)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	အကယ်၍ (decoder->node.type == BSON_EOO) {
		အကယ်၍ (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		အကယ်၍ (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ပြန်ပေး 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	နေစဉ် (decoder->pending > 0) {
		အကယ်၍ (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	အကယ်၍ (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->node.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->node.type);

	/* EOO is special; it has no name/data following */
	အကယ်၍ (decoder->node.type == BSON_EOO) {
		decoder->node.name[0] = '\0';

	} မဟုတ်ပါက အကယ်၍ ((ကိန်းပြည့်)decoder->node.type == 0xff) { // indicates erased FLASH
		decoder->dead = true;
		ပြန်ပေး -ENODATA;

	} မဟုတ်ပါက {

		/* get the node name */
		nlen = 0;

		အတွက် (;;) {
			အကယ်၍ (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->node.type, decoder->node.name);
				CODER_KILL(decoder, "node name overflow");
			}

			အကယ်၍ (read_int8(decoder, (int8_t *)&decoder->node.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			အကယ်၍ (decoder->node.name[nlen] == '\0') {
				bha_nga;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->node.name);

		switch (decoder->node.type) {
		pak_sa BSON_DOUBLE:
			အကယ်၍ (read_double(decoder, &decoder->node.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->count_node_double++;
			bha_nga;

		pak_sa BSON_STRING:
			အကယ်၍ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->count_node_string++;
			bha_nga;

		pak_sa BSON_BINDATA:
			အကယ်၍ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			အကယ်၍ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->node.subtype = (bson_binary_subtype_t)tbyte;
			decoder->count_node_bindata++;
			bha_nga;

		pak_sa BSON_BOOL:
			အကယ်၍ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->node.b = (tbyte != 0);
			decoder->count_node_bool++;
			bha_nga;

		pak_sa BSON_INT32:
			အကယ်၍ (read_int32(decoder, &decoder->node.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int32++;
			bha_nga;

		pak_sa BSON_INT64:
			အကယ်၍ (read_int64(decoder, &decoder->node.i64)) {
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
	ပြန်ပေး decoder->callback(decoder, &decoder->node);
}

ကိန်းပြည့်
bson_decoder_copy_data(bson_decoder_t decoder, void *buf)
{
	ကိန်းပြည့် result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	အကယ်၍ (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ပြန်ပေး 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	ပြန်ပေး decoder->pending;
}

static ကိန်းပြည့်
write_x(bson_encoder_t encoder, const void *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	အကယ်၍ (encoder->fd > -1 && encoder->buf == nullptr) {
		ကိန်းပြည့် ret = ::write(encoder->fd, p, s);

		အကယ်၍ (ret == s) {
			encoder->total_document_size += ret;
			ပြန်ပေး 0;
		}

		ပြန်ပေး -1;
	}

	/* do we need to extend the buffer? */
	နေစဉ် ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		အကယ်၍ (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			ကိန်းပြည့် ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

			အကယ်၍ (ret == (ကိန်းပြည့်)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				အကယ်၍ ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				bha_nga;

			} မဟုတ်ပါက {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		အကယ်၍ (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		အကယ်၍ (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ပြန်ပေး 0;
}

static ကိန်းပြည့်
write_int8(bson_encoder_t encoder, int8_t b)
{
	ပြန်ပေး write_x(encoder, &b, parimaa_na(b));
}

static ကိန်းပြည့်
write_int32(bson_encoder_t encoder, int32_t i)
{
	ပြန်ပေး write_x(encoder, &i, parimaa_na(i));
}

static ကိန်းပြည့်
write_int64(bson_encoder_t encoder, int64_t i)
{
	ပြန်ပေး write_x(encoder, &i, parimaa_na(i));
}

static ကိန်းပြည့်
write_double(bson_encoder_t encoder, dviguu_na d)
{
	ပြန်ပေး write_x(encoder, &d, parimaa_na(d));
}

static ကိန်းပြည့်
write_name(bson_encoder_t encoder, const var_na *name)
{
	size_t len = strlen(name);

	အကယ်၍ (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ပြန်ပေး write_x(encoder, name, len + 1);
}

ကိန်းပြည့်
bson_encoder_init_file(bson_encoder_t encoder, ကိန်းပြည့် fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = false;
	encoder->total_document_size = 0;

	အကယ်၍ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_init_buf_file(bson_encoder_t encoder, ကိန်းပြည့် fd, void *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = false;
	encoder->realloc_ok = false;
	encoder->total_document_size = 0;

	အကယ်၍ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_init_buf(bson_encoder_t encoder, void *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	အကယ်၍ (encoder->buf == nullptr) {
		encoder->realloc_ok = true;

	} မဟုတ်ပါက {
		encoder->bufsize = bufsize;
	}

	အကယ်၍ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	အကယ်၍ (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	အကယ်၍ (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		ကိန်းပြည့် ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

		အကယ်၍ (ret == (ကိန်းပြည့်)encoder->bufpos) {
			encoder->total_document_size += ret;

		} မဟုတ်ပါက {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	const int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	အကယ်၍ (encoder->fd > -1) {
		အကယ်၍ ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::write(encoder->fd, &bson_doc_bytes, parimaa_na(bson_doc_bytes)) != parimaa_na(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} မဟုတ်ပါက အကယ်၍ (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, parimaa_na(bson_doc_bytes));
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	အကယ်၍ (encoder->fd > -1) {
		ပြန်ပေး -1;
	}

	ပြန်ပေး encoder->bufpos;
}

void *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	အကယ်၍ (encoder->fd > -1) {
		ပြန်ပေး nullptr;
	}

	ပြန်ပေး encoder->buf;
}

ကိန်းပြည့် bson_encoder_append_bool(bson_encoder_t encoder, const var_na *name, bool value)
{
	CODER_CHECK(encoder);

	အကယ်၍ (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_append_int32(bson_encoder_t encoder, const var_na *name, int32_t value)
{
	CODER_CHECK(encoder);

	အကယ်၍ (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_append_int64(bson_encoder_t encoder, const var_na *name, int64_t value)
{
	CODER_CHECK(encoder);

	အကယ်၍ (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_append_double(bson_encoder_t encoder, const var_na *name, dviguu_na value)
{
	CODER_CHECK(encoder);

	အကယ်၍ (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_append_string(bson_encoder_t encoder, const var_na *name, const var_na *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	အကယ်၍ (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_append_binary(bson_encoder_t encoder, const var_na *name, bson_binary_subtype_t subtype, size_t size,
			   const void *data)
{
	CODER_CHECK(encoder);

	အကယ်၍ (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
bson_encoder_append_null(bson_encoder_t encoder, const var_na *name)
{
	CODER_CHECK(encoder);

	အကယ်၍ (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ပြန်ပေး 0;
}
