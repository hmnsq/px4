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

#නම් 0
# define debug(fmt, args...)		do { PX4_INFO("BSON: " fmt, ##args); } අතරතුර(0)
#නැතහොත්
# define debug(fmt, args...)		do { } අතරතුර(0)
#endif

#define CODER_CHECK(_c)		do { නම් ((_c)->dead) { PX4_ERR("coder dead"); ආපසු -1; }} අතරතුර(0)
#define CODER_KILL(_c, _reason)	do { PX4_ERR("killed: %s", _reason); (_c)->dead = true; ආපසු -1; } අතරතුර(0)

static පූර්ණ_සංඛ්‍යා
read_x(bson_decoder_t decoder, void *p, size_t s)
{
	CODER_CHECK(decoder);

	නම් (decoder->fd > -1) {
		පූර්ණ_සංඛ්‍යා ret = ::read(decoder->fd, p, s);

		නම් (ret == s) {
			decoder->total_decoded_size += ret;
			ආපසු 0;
		}

		ආපසු -1;
	}

	නම් (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		නම් (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		නම් ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		ආපසු 0;
	}

	debug("no source");
	ආපසු -1;
}

static පූර්ණ_සංඛ්‍යා
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ආපසු read_x(decoder, b, parimaa_na(*b));
}

static පූර්ණ_සංඛ්‍යා
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ආපසු read_x(decoder, i, parimaa_na(*i));
}

static පූර්ණ_සංඛ්‍යා
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ආපසු read_x(decoder, i, parimaa_na(*i));
}

static පූර්ණ_සංඛ්‍යා
read_double(bson_decoder_t decoder, dviguu_na *d)
{
	ආපසු read_x(decoder, d, parimaa_na(*d));
}

පූර්ණ_සංඛ්‍යා
bson_decoder_init_file(bson_decoder_t decoder, පූර්ණ_සංඛ්‍යා fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->node.type = BSON_UNDEFINED;

	// read document size
	නම් (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	නම් (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_decoder_init_buf(bson_decoder_t decoder, void *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	නම් ((buf == nullptr) || (callback == nullptr)) {
		ආපසු -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = false;

	නම් (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} නැතහොත් {
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

	නම් (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	නම් (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	නම් ((decoder->total_document_size > 0) && (decoder->total_document_size > (පූර්ණ_සංඛ්‍යා)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	නම් (decoder->node.type == BSON_EOO) {
		නම් (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		නම් (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ආපසු 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	අතරතුර (decoder->pending > 0) {
		නම් (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	නම් (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->node.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->node.type);

	/* EOO is special; it has no name/data following */
	නම් (decoder->node.type == BSON_EOO) {
		decoder->node.name[0] = '\0';

	} නැතහොත් නම් ((පූර්ණ_සංඛ්‍යා)decoder->node.type == 0xff) { // indicates erased FLASH
		decoder->dead = true;
		ආපසු -ENODATA;

	} නැතහොත් {

		/* get the node name */
		nlen = 0;

		සඳහා (;;) {
			නම් (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->node.type, decoder->node.name);
				CODER_KILL(decoder, "node name overflow");
			}

			නම් (read_int8(decoder, (int8_t *)&decoder->node.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			නම් (decoder->node.name[nlen] == '\0') {
				bha_nga;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->node.name);

		switch (decoder->node.type) {
		pak_sa BSON_DOUBLE:
			නම් (read_double(decoder, &decoder->node.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->count_node_double++;
			bha_nga;

		pak_sa BSON_STRING:
			නම් (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->count_node_string++;
			bha_nga;

		pak_sa BSON_BINDATA:
			නම් (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			නම් (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->node.subtype = (bson_binary_subtype_t)tbyte;
			decoder->count_node_bindata++;
			bha_nga;

		pak_sa BSON_BOOL:
			නම් (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->node.b = (tbyte != 0);
			decoder->count_node_bool++;
			bha_nga;

		pak_sa BSON_INT32:
			නම් (read_int32(decoder, &decoder->node.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int32++;
			bha_nga;

		pak_sa BSON_INT64:
			නම් (read_int64(decoder, &decoder->node.i64)) {
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
	ආපසු decoder->callback(decoder, &decoder->node);
}

පූර්ණ_සංඛ්‍යා
bson_decoder_copy_data(bson_decoder_t decoder, void *buf)
{
	පූර්ණ_සංඛ්‍යා result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	නම් (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ආපසු 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	ආපසු decoder->pending;
}

static පූර්ණ_සංඛ්‍යා
write_x(bson_encoder_t encoder, const void *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	නම් (encoder->fd > -1 && encoder->buf == nullptr) {
		පූර්ණ_සංඛ්‍යා ret = ::write(encoder->fd, p, s);

		නම් (ret == s) {
			encoder->total_document_size += ret;
			ආපසු 0;
		}

		ආපසු -1;
	}

	/* do we need to extend the buffer? */
	අතරතුර ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		නම් (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			පූර්ණ_සංඛ්‍යා ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

			නම් (ret == (පූර්ණ_සංඛ්‍යා)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				නම් ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				bha_nga;

			} නැතහොත් {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		නම් (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		නම් (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා
write_int8(bson_encoder_t encoder, int8_t b)
{
	ආපසු write_x(encoder, &b, parimaa_na(b));
}

static පූර්ණ_සංඛ්‍යා
write_int32(bson_encoder_t encoder, int32_t i)
{
	ආපසු write_x(encoder, &i, parimaa_na(i));
}

static පූර්ණ_සංඛ්‍යා
write_int64(bson_encoder_t encoder, int64_t i)
{
	ආපසු write_x(encoder, &i, parimaa_na(i));
}

static පූර්ණ_සංඛ්‍යා
write_double(bson_encoder_t encoder, dviguu_na d)
{
	ආපසු write_x(encoder, &d, parimaa_na(d));
}

static පූර්ණ_සංඛ්‍යා
write_name(bson_encoder_t encoder, const var_na *name)
{
	size_t len = strlen(name);

	නම් (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ආපසු write_x(encoder, name, len + 1);
}

පූර්ණ_සංඛ්‍යා
bson_encoder_init_file(bson_encoder_t encoder, පූර්ණ_සංඛ්‍යා fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = false;
	encoder->total_document_size = 0;

	නම් (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_init_buf_file(bson_encoder_t encoder, පූර්ණ_සංඛ්‍යා fd, void *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = false;
	encoder->realloc_ok = false;
	encoder->total_document_size = 0;

	නම් (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_init_buf(bson_encoder_t encoder, void *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	නම් (encoder->buf == nullptr) {
		encoder->realloc_ok = true;

	} නැතහොත් {
		encoder->bufsize = bufsize;
	}

	නම් (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	නම් (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	නම් (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		පූර්ණ_සංඛ්‍යා ret = ::write(encoder->fd, encoder->buf, encoder->bufpos);

		නම් (ret == (පූර්ණ_සංඛ්‍යා)encoder->bufpos) {
			encoder->total_document_size += ret;

		} නැතහොත් {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	const int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	නම් (encoder->fd > -1) {
		නම් ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::write(encoder->fd, &bson_doc_bytes, parimaa_na(bson_doc_bytes)) != parimaa_na(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} නැතහොත් නම් (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, parimaa_na(bson_doc_bytes));
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	නම් (encoder->fd > -1) {
		ආපසු -1;
	}

	ආපසු encoder->bufpos;
}

void *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	නම් (encoder->fd > -1) {
		ආපසු nullptr;
	}

	ආපසු encoder->buf;
}

පූර්ණ_සංඛ්‍යා bson_encoder_append_bool(bson_encoder_t encoder, const var_na *name, bool value)
{
	CODER_CHECK(encoder);

	නම් (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_append_int32(bson_encoder_t encoder, const var_na *name, int32_t value)
{
	CODER_CHECK(encoder);

	නම් (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_append_int64(bson_encoder_t encoder, const var_na *name, int64_t value)
{
	CODER_CHECK(encoder);

	නම් (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_append_double(bson_encoder_t encoder, const var_na *name, dviguu_na value)
{
	CODER_CHECK(encoder);

	නම් (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_append_string(bson_encoder_t encoder, const var_na *name, const var_na *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	නම් (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_append_binary(bson_encoder_t encoder, const var_na *name, bson_binary_subtype_t subtype, size_t size,
			   const void *data)
{
	CODER_CHECK(encoder);

	නම් (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා
bson_encoder_append_null(bson_encoder_t encoder, const var_na *name)
{
	CODER_CHECK(encoder);

	නම් (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ආපසු 0;
}
