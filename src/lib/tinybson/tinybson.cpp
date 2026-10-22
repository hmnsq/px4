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
# define debug(fmt, دليلون...)		افعل { PX4_INFO("BSON: " fmt, ##args); } طالما(0)
#وإلا
# define debug(fmt, دليلون...)		افعل { } طالما(0)
#جيڪڏهن_پورو

#وصف_ڪريو CODER_CHECK(_c)		افعل { إذا ((_c)->dead) { PX4_ERR("coder dead"); أرجع -1; }} طالما(0)
#وصف_ڪريو CODER_KILL(_c, _reason)	افعل { PX4_ERR("killed: %s", _reason); (_c)->dead = سچ; أرجع -1; } طالما(0)

ساكن صحيح
read_x(bson_decoder_t decoder, فراغ *p, size_t s)
{
	CODER_CHECK(decoder);

	إذا (decoder->fd > -1) {
		صحيح ret = ::اقرأ(decoder->fd, p, s);

		إذا (ret == s) {
			decoder->ڪل_decoded_سائيز += ret;
			أرجع 0;
		}

		أرجع -1;
	}

	إذا (decoder->buf != خالي_اشارو) {
		/* staged operations to avoid integer overflow for corrupt data */
		إذا (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		إذا ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		يادگيري_نقل(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->ڪل_decoded_سائيز += s;
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
	decoder->ڳنڍ_2.type = BSON_UNDEFINED;

	// read document size
	إذا (read_int32(decoder, &decoder->ڪل_document_سائيز)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->ڪل_document_سائيز);

	// The length is signed and comes from the document itself
	إذا (decoder->ڪل_document_سائيز < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	أرجع 0;
}

صحيح
bson_decoder_init_buf(bson_decoder_t decoder, فراغ *buf, بنانشان bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	إذا ((buf == خالي_اشارو) || (callback == خالي_اشارو)) {
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
	decoder->ڳنڍ_2.type = BSON_UNDEFINED;
	decoder->ڪل_decoded_سائيز = 0;

	// read document size
	decoder->ڪل_document_سائيز = 0;

	إذا (read_int32(decoder, &decoder->ڪل_document_سائيز)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->ڪل_document_سائيز);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	إذا (decoder->ڪل_document_سائيز < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	إذا ((decoder->ڪل_document_سائيز > 0) && (decoder->ڪل_document_سائيز > (صحيح)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	أرجع 0;
}

صحيح
bson_decoder_ايندڙ(bson_decoder_t decoder)
{
	int8_t	tbyte;
	بنانشان nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	إذا (decoder->ڳنڍ_2.type == BSON_EOO) {
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

	decoder->ڳنڍ_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->ڳنڍ_2.type);

	/* EOO is special; it has no name/data following */
	إذا (decoder->ڳنڍ_2.type == BSON_EOO) {
		decoder->ڳنڍ_2.name[0] = '\0';

	} وإلا إذا ((صحيح)decoder->ڳنڍ_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = سچ;
		أرجع -ENODATA;

	} وإلا {

		/* get the node name */
		nlen = 0;

		لكل (;;) {
			إذا (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->ڳنڍ_2.type, decoder->ڳنڍ_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			إذا (read_int8(decoder, (int8_t *)&decoder->ڳنڍ_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			إذا (decoder->ڳنڍ_2.name[nlen] == '\0') {
				اكسر;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->ڳنڍ_2.name);

		بدل (decoder->ڳنڍ_2.type) {
		حالة BSON_DOUBLE:
			إذا (read_double(decoder, &decoder->ڳنڍ_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->ڳڻپ_ڳنڍ_double++;
			اكسر;

		حالة BSON_STRING:
			إذا (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->ڳڻپ_ڳنڍ_string++;
			اكسر;

		حالة BSON_BINDATA:
			إذا (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			إذا (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->ڳنڍ_2.subtype = (bson_ٻٽو_subtype_t)tbyte;
			decoder->ڳڻپ_ڳنڍ_bindata++;
			اكسر;

		حالة BSON_BOOL:
			إذا (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->ڳنڍ_2.b = (tbyte != 0);
			decoder->ڳڻپ_ڳنڍ_bool++;
			اكسر;

		حالة BSON_INT32:
			إذا (read_int32(decoder, &decoder->ڳنڍ_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ڳڻپ_ڳنڍ_int32++;
			اكسر;

		حالة BSON_INT64:
			إذا (read_int64(decoder, &decoder->ڳنڍ_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ڳڻپ_ڳنڍ_int64++;
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
	أرجع decoder->callback(decoder, &decoder->ڳنڍ_2);
}

صحيح
bson_decoder_copy_معلومات(bson_decoder_t decoder, فراغ *buf)
{
	صحيح نتيجو;

	CODER_CHECK(decoder);

	/* copy data */
	نتيجو = read_x(decoder, buf, decoder->pending);

	إذا (نتيجو != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	أرجع 0;
}

size_t
bson_decoder_معلومات_pending(bson_decoder_t decoder)
{
	أرجع decoder->pending;
}

ساكن صحيح
write_x(bson_encoder_t encoder, ثابت فراغ *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	إذا (encoder->fd > -1 && encoder->buf == خالي_اشارو) {
		صحيح ret = ::اكتب(encoder->fd, p, s);

		إذا (ret == s) {
			encoder->ڪل_document_سائيز += ret;
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
				encoder->ڪل_document_سائيز += ret;

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

		إذا (newbuf == خالي_اشارو) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	يادگيري_نقل(encoder->buf + encoder->bufpos, p, s);
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
	size_t len = لڙي_ڊيگهه(name);

	إذا (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	أرجع write_x(encoder, name, len + 1);
}

صحيح
bson_encoder_init_file(bson_encoder_t encoder, صحيح fd)
{
	encoder->fd = fd;
	encoder->buf = خالي_اشارو;
	encoder->dead = خطأ;
	encoder->ڪل_document_سائيز = 0;

	إذا (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	أرجع 0;
}

صحيح
bson_encoder_init_buf_file(bson_encoder_t encoder, صحيح fd, فراغ *buf, بنانشان bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = خطأ;
	encoder->realloc_ok = خطأ;
	encoder->ڪل_document_سائيز = 0;

	إذا (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	أرجع 0;
}

صحيح
bson_encoder_init_buf(bson_encoder_t encoder, فراغ *buf, بنانشان bufsize)
{
	encoder->buf = (uint8_t *)buf;

	إذا (encoder->buf == خالي_اشارو) {
		encoder->realloc_ok = سچ;

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

	إذا (encoder->fd > -1 && encoder->buf != خالي_اشارو && encoder->bufpos > 0) {
		/* write final buffer to disk */
		صحيح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

		إذا (ret == (صحيح)encoder->bufpos) {
			encoder->ڪل_document_سائيز += ret;

		} وإلا {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ثابت int32_t bson_doc_بائيٽون = (encoder->fd > -1) ? encoder->ڪل_document_سائيز : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_بائيٽون);

	إذا (encoder->fd > -1) {
		إذا ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::اكتب(encoder->fd, &bson_doc_بائيٽون, حجم(bson_doc_بائيٽون)) != حجم(bson_doc_بائيٽون))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} وإلا إذا (encoder->buf != خالي_اشارو) {
		/* update buffer length */
		يادگيري_نقل(encoder->buf, &bson_doc_بائيٽون, حجم(bson_doc_بائيٽون));
	}

	أرجع 0;
}

صحيح
bson_encoder_buf_سائيز(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	إذا (encoder->fd > -1) {
		أرجع -1;
	}

	أرجع encoder->bufpos;
}

فراغ *
bson_encoder_buf_معلومات(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	إذا (encoder->fd > -1) {
		أرجع خالي_اشارو;
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

	len = لڙي_ڊيگهه(string) + 1;	/* include trailing nul */

	إذا (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	أرجع 0;
}

صحيح
bson_encoder_append_ٻٽو(bson_encoder_t encoder, ثابت حرف *name, bson_ٻٽو_subtype_t subtype, size_t سائيز,
			   ثابت فراغ *معلومات)
{
	CODER_CHECK(encoder);

	إذا (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, سائيز) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, معلومات, سائيز)) {
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
