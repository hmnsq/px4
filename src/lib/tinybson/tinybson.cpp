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

#إذا 0
# define debug(fmt, args...)		افعل { PX4_INFO("BSON: " fmt, ##args); } طالما(0)
#وإلا
# define debug(fmt, args...)		افعل { } طالما(0)
#endif

#define CODER_CHECK(_c)		افعل { إذا ((_c)->dead) { PX4_ERR("coder dead"); أرجع -1; }} طالما(0)
#define CODER_KILL(_c, _reason)	افعل { PX4_ERR("killed: %s", _reason); (_c)->dead = true; أرجع -1; } طالما(0)

ساكن صحيح
read_x(bson_decoder_t decoder, فراغ *p, size_t s)
{
	CODER_CHECK(decoder);

	إذا (decoder->fd > -1) {
		صحيح ret = ::اقرأ(decoder->fd, p, s);

		إذا (ret == s) {
			decoder->total_decoded_size += ret;
			أرجع 0;
		}

		أرجع -1;
	}

	إذا (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		إذا (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		إذا ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		أرجع 0;
	}

	debug("no source");
	أرجع -1;
}

ساكن صحيح
read_int8(bson_decoder_t decoder, int8_t *b)
{
	أرجع read_x(decoder, b, حجم(*b));
}

ساكن صحيح
read_int32(bson_decoder_t decoder, int32_t *i)
{
	أرجع read_x(decoder, i, حجم(*i));
}

ساكن صحيح
read_int64(bson_decoder_t decoder, int64_t *i)
{
	أرجع read_x(decoder, i, حجم(*i));
}

ساكن صحيح
read_double(bson_decoder_t decoder, مزدوج *d)
{
	أرجع read_x(decoder, d, حجم(*d));
}

صحيح
bson_decoder_init_file(bson_decoder_t decoder, صحيح fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->تۈگۈن_2.type = BSON_UNDEFINED;

	// read document size
	إذا (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	إذا (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	أرجع 0;
}

صحيح
bson_decoder_init_buf(bson_decoder_t decoder, فراغ *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	إذا ((buf == nullptr) || (callback == nullptr)) {
		أرجع -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = خطأ;

	إذا (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} وإلا {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->تۈگۈن_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	إذا (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	إذا (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	إذا ((decoder->total_document_size > 0) && (decoder->total_document_size > (صحيح)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	أرجع 0;
}

صحيح
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	إذا (decoder->تۈگۈن_2.type == BSON_EOO) {
		إذا (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		إذا (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			أرجع 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	طالما (decoder->pending > 0) {
		إذا (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	إذا (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->تۈگۈن_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->تۈگۈن_2.type);

	/* EOO is special; it has no name/data following */
	إذا (decoder->تۈگۈن_2.type == BSON_EOO) {
		decoder->تۈگۈن_2.name[0] = '\0';

	} وإلا إذا ((صحيح)decoder->تۈگۈن_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = true;
		أرجع -ENODATA;

	} وإلا {

		/* get the node name */
		nlen = 0;

		لكل (;;) {
			إذا (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->تۈگۈن_2.type, decoder->تۈگۈن_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			إذا (read_int8(decoder, (int8_t *)&decoder->تۈگۈن_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			إذا (decoder->تۈگۈن_2.name[nlen] == '\0') {
				اكسر;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->تۈگۈن_2.name);

		بدل (decoder->تۈگۈن_2.type) {
		حالة BSON_DOUBLE:
			إذا (read_double(decoder, &decoder->تۈگۈن_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->سانى_تۈگۈن_double++;
			اكسر;

		حالة BSON_STRING:
			إذا (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->سانى_تۈگۈن_string++;
			اكسر;

		حالة BSON_BINDATA:
			إذا (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			إذا (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->تۈگۈن_2.subtype = (bson_ئىككىلىك_subtype_t)tbyte;
			decoder->سانى_تۈگۈن_bindata++;
			اكسر;

		حالة BSON_BOOL:
			إذا (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->تۈگۈن_2.b = (tbyte != 0);
			decoder->سانى_تۈگۈن_bool++;
			اكسر;

		حالة BSON_INT32:
			إذا (read_int32(decoder, &decoder->تۈگۈن_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->سانى_تۈگۈن_int32++;
			اكسر;

		حالة BSON_INT64:
			إذا (read_int64(decoder, &decoder->تۈگۈن_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->سانى_تۈگۈن_int64++;
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
	أرجع decoder->callback(decoder, &decoder->تۈگۈن_2);
}

صحيح
bson_decoder_copy_data(bson_decoder_t decoder, فراغ *buf)
{
	صحيح result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	إذا (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	أرجع 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	أرجع decoder->pending;
}

ساكن صحيح
write_x(bson_encoder_t encoder, ثابت فراغ *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	إذا (encoder->fd > -1 && encoder->buf == nullptr) {
		صحيح ret = ::اكتب(encoder->fd, p, s);

		إذا (ret == s) {
			encoder->total_document_size += ret;
			أرجع 0;
		}

		أرجع -1;
	}

	/* do we need to extend the buffer? */
	طالما ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		إذا (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			صحيح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

			إذا (ret == (صحيح)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				إذا ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				اكسر;

			} وإلا {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		إذا (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		إذا (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	أرجع 0;
}

ساكن صحيح
write_int8(bson_encoder_t encoder, int8_t b)
{
	أرجع write_x(encoder, &b, حجم(b));
}

ساكن صحيح
write_int32(bson_encoder_t encoder, int32_t i)
{
	أرجع write_x(encoder, &i, حجم(i));
}

ساكن صحيح
write_int64(bson_encoder_t encoder, int64_t i)
{
	أرجع write_x(encoder, &i, حجم(i));
}

ساكن صحيح
write_double(bson_encoder_t encoder, مزدوج d)
{
	أرجع write_x(encoder, &d, حجم(d));
}

ساكن صحيح
write_name(bson_encoder_t encoder, ثابت حرف *name)
{
	size_t len = strlen(name);

	إذا (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	أرجع write_x(encoder, name, len + 1);
}

صحيح
bson_encoder_init_file(bson_encoder_t encoder, صحيح fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = خطأ;
	encoder->total_document_size = 0;

	إذا (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	أرجع 0;
}

صحيح
bson_encoder_init_buf_file(bson_encoder_t encoder, صحيح fd, فراغ *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = خطأ;
	encoder->realloc_ok = خطأ;
	encoder->total_document_size = 0;

	إذا (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	أرجع 0;
}

صحيح
bson_encoder_init_buf(bson_encoder_t encoder, فراغ *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	إذا (encoder->buf == nullptr) {
		encoder->realloc_ok = true;

	} وإلا {
		encoder->bufsize = bufsize;
	}

	إذا (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	أرجع 0;
}

صحيح
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	إذا (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		صحيح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

		إذا (ret == (صحيح)encoder->bufpos) {
			encoder->total_document_size += ret;

		} وإلا {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ثابت int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	إذا (encoder->fd > -1) {
		إذا ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::اكتب(encoder->fd, &bson_doc_bytes, حجم(bson_doc_bytes)) != حجم(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} وإلا إذا (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, حجم(bson_doc_bytes));
	}

	أرجع 0;
}

صحيح
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	إذا (encoder->fd > -1) {
		أرجع -1;
	}

	أرجع encoder->bufpos;
}

فراغ *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	إذا (encoder->fd > -1) {
		أرجع nullptr;
	}

	أرجع encoder->buf;
}

صحيح bson_encoder_append_bool(bson_encoder_t encoder, ثابت حرف *name, منطقي value)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	أرجع 0;
}

صحيح
bson_encoder_append_int32(bson_encoder_t encoder, ثابت حرف *name, int32_t value)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	أرجع 0;
}

صحيح
bson_encoder_append_int64(bson_encoder_t encoder, ثابت حرف *name, int64_t value)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	أرجع 0;
}

صحيح
bson_encoder_append_double(bson_encoder_t encoder, ثابت حرف *name, مزدوج value)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	أرجع 0;
}

صحيح
bson_encoder_append_string(bson_encoder_t encoder, ثابت حرف *name, ثابت حرف *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	إذا (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	أرجع 0;
}

صحيح
bson_encoder_append_ئىككىلىك(bson_encoder_t encoder, ثابت حرف *name, bson_ئىككىلىك_subtype_t subtype, size_t size,
			   ثابت فراغ *data)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	أرجع 0;
}

صحيح
bson_encoder_append_null(bson_encoder_t encoder, ثابت حرف *name)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	أرجع 0;
}
