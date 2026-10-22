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

#ضمن <px4_platform_common/posix.h>
#ضمن <unistd.h>
#ضمن <string.h>
#ضمن <stdlib.h>
#ضمن <systemlib/err.h>

#ضمن "tinybson.h"

#ܐܢ 0
# define debug(fmt, args...)		ܥܒܕ { PX4_INFO("BSON: " fmt, ##args); } ܟܡܐ(0)
#ܐܚܪܢܐ
# define debug(fmt, args...)		ܥܒܕ { } ܟܡܐ(0)
#endif

#define CODER_CHECK(_c)		ܥܒܕ { ܐܢ ((_c)->dead) { PX4_ERR("coder dead"); ܦܢܐ -1; }} ܟܡܐ(0)
#define CODER_KILL(_c, _reason)	ܥܒܕ { PX4_ERR("killed: %s", _reason); (_c)->dead = ܫܪܪܐ; ܦܢܐ -1; } ܟܡܐ(0)

ساكن صحيح
read_x(bson_decoder_t decoder, فراغ *p, size_t s)
{
	CODER_CHECK(decoder);

	ܐܢ (decoder->fd > -1) {
		صحيح ret = ::اقرأ(decoder->fd, p, s);

		ܐܢ (ret == s) {
			decoder->total_decoded_size += ret;
			ܦܢܐ 0;
		}

		ܦܢܐ -1;
	}

	ܐܢ (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		ܐܢ (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ܐܢ ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		ܦܢܐ 0;
	}

	debug("no source");
	ܦܢܐ -1;
}

ساكن صحيح
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ܦܢܐ read_x(decoder, b, حجم(*b));
}

ساكن صحيح
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ܦܢܐ read_x(decoder, i, حجم(*i));
}

ساكن صحيح
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ܦܢܐ read_x(decoder, i, حجم(*i));
}

ساكن صحيح
read_double(bson_decoder_t decoder, مزدوج *d)
{
	ܦܢܐ read_x(decoder, d, حجم(*d));
}

صحيح
bson_decoder_init_file(bson_decoder_t decoder, صحيح fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->node.type = BSON_UNDEFINED;

	// read document size
	ܐܢ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	ܐܢ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ܦܢܐ 0;
}

صحيح
bson_decoder_init_buf(bson_decoder_t decoder, فراغ *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ܐܢ ((buf == nullptr) || (callback == nullptr)) {
		ܦܢܐ -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = ܕܓܠܐ;

	ܐܢ (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} ܐܚܪܢܐ {
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

	ܐܢ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ܐܢ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ܐܢ ((decoder->total_document_size > 0) && (decoder->total_document_size > (صحيح)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ܦܢܐ 0;
}

صحيح
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ܐܢ (decoder->node.type == BSON_EOO) {
		ܐܢ (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ܐܢ (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ܦܢܐ 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	ܟܡܐ (decoder->pending > 0) {
		ܐܢ (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ܐܢ (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->node.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->node.type);

	/* EOO is special; it has no name/data following */
	ܐܢ (decoder->node.type == BSON_EOO) {
		decoder->node.name[0] = '\0';

	} ܐܚܪܢܐ ܐܢ ((صحيح)decoder->node.type == 0xff) { // indicates erased FLASH
		decoder->dead = ܫܪܪܐ;
		ܦܢܐ -ENODATA;

	} ܐܚܪܢܐ {

		/* get the node name */
		nlen = 0;

		لكل (;;) {
			ܐܢ (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->node.type, decoder->node.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ܐܢ (read_int8(decoder, (int8_t *)&decoder->node.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ܐܢ (decoder->node.name[nlen] == '\0') {
				اكسر;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->node.name);

		بدل (decoder->node.type) {
		حالة BSON_DOUBLE:
			ܐܢ (read_double(decoder, &decoder->node.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->count_node_double++;
			اكسر;

		حالة BSON_STRING:
			ܐܢ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->count_node_string++;
			اكسر;

		حالة BSON_BINDATA:
			ܐܢ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ܐܢ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->node.subtype = (bson_binary_subtype_t)tbyte;
			decoder->count_node_bindata++;
			اكسر;

		حالة BSON_BOOL:
			ܐܢ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->node.b = (tbyte != 0);
			decoder->count_node_bool++;
			اكسر;

		حالة BSON_INT32:
			ܐܢ (read_int32(decoder, &decoder->node.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int32++;
			اكسر;

		حالة BSON_INT64:
			ܐܢ (read_int64(decoder, &decoder->node.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->count_node_int64++;
			اكسر;

		حالة BSON_nullptr:
		حالة BSON_UNDEFINED:
			اكسر;

		/* XXX currently not supporting other types */
		افتراضي:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	ܦܢܐ decoder->callback(decoder, &decoder->node);
}

صحيح
bson_decoder_copy_data(bson_decoder_t decoder, فراغ *buf)
{
	صحيح result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	ܐܢ (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ܦܢܐ 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	ܦܢܐ decoder->pending;
}

ساكن صحيح
write_x(bson_encoder_t encoder, ثابت فراغ *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ܐܢ (encoder->fd > -1 && encoder->buf == nullptr) {
		صحيح ret = ::اكتب(encoder->fd, p, s);

		ܐܢ (ret == s) {
			encoder->total_document_size += ret;
			ܦܢܐ 0;
		}

		ܦܢܐ -1;
	}

	/* do we need to extend the buffer? */
	ܟܡܐ ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ܐܢ (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			صحيح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

			ܐܢ (ret == (صحيح)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				ܐܢ ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				اكسر;

			} ܐܚܪܢܐ {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ܐܢ (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ܐܢ (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ܦܢܐ 0;
}

ساكن صحيح
write_int8(bson_encoder_t encoder, int8_t b)
{
	ܦܢܐ write_x(encoder, &b, حجم(b));
}

ساكن صحيح
write_int32(bson_encoder_t encoder, int32_t i)
{
	ܦܢܐ write_x(encoder, &i, حجم(i));
}

ساكن صحيح
write_int64(bson_encoder_t encoder, int64_t i)
{
	ܦܢܐ write_x(encoder, &i, حجم(i));
}

ساكن صحيح
write_double(bson_encoder_t encoder, مزدوج d)
{
	ܦܢܐ write_x(encoder, &d, حجم(d));
}

ساكن صحيح
write_name(bson_encoder_t encoder, ثابت حرف *name)
{
	size_t len = strlen(name);

	ܐܢ (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ܦܢܐ write_x(encoder, name, len + 1);
}

صحيح
bson_encoder_init_file(bson_encoder_t encoder, صحيح fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = ܕܓܠܐ;
	encoder->total_document_size = 0;

	ܐܢ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_init_buf_file(bson_encoder_t encoder, صحيح fd, فراغ *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = ܕܓܠܐ;
	encoder->realloc_ok = ܕܓܠܐ;
	encoder->total_document_size = 0;

	ܐܢ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_init_buf(bson_encoder_t encoder, فراغ *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ܐܢ (encoder->buf == nullptr) {
		encoder->realloc_ok = ܫܪܪܐ;

	} ܐܚܪܢܐ {
		encoder->bufsize = bufsize;
	}

	ܐܢ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ܐܢ (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ܐܢ (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		صحيح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

		ܐܢ (ret == (صحيح)encoder->bufpos) {
			encoder->total_document_size += ret;

		} ܐܚܪܢܐ {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ثابت int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	ܐܢ (encoder->fd > -1) {
		ܐܢ ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::اكتب(encoder->fd, &bson_doc_bytes, حجم(bson_doc_bytes)) != حجم(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} ܐܚܪܢܐ ܐܢ (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, حجم(bson_doc_bytes));
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ܐܢ (encoder->fd > -1) {
		ܦܢܐ -1;
	}

	ܦܢܐ encoder->bufpos;
}

فراغ *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ܐܢ (encoder->fd > -1) {
		ܦܢܐ nullptr;
	}

	ܦܢܐ encoder->buf;
}

صحيح bson_encoder_append_bool(bson_encoder_t encoder, ثابت حرف *name, منطقي value)
{
	CODER_CHECK(encoder);

	ܐܢ (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_append_int32(bson_encoder_t encoder, ثابت حرف *name, int32_t value)
{
	CODER_CHECK(encoder);

	ܐܢ (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_append_int64(bson_encoder_t encoder, ثابت حرف *name, int64_t value)
{
	CODER_CHECK(encoder);

	ܐܢ (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_append_double(bson_encoder_t encoder, ثابت حرف *name, مزدوج value)
{
	CODER_CHECK(encoder);

	ܐܢ (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_append_string(bson_encoder_t encoder, ثابت حرف *name, ثابت حرف *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	ܐܢ (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_append_binary(bson_encoder_t encoder, ثابت حرف *name, bson_binary_subtype_t subtype, size_t size,
			   ثابت فراغ *data)
{
	CODER_CHECK(encoder);

	ܐܢ (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ܦܢܐ 0;
}

صحيح
bson_encoder_append_null(bson_encoder_t encoder, ثابت حرف *name)
{
	CODER_CHECK(encoder);

	ܐܢ (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ܦܢܐ 0;
}
