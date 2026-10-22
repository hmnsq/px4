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

#شامل <px4_platform_common/posix.h>
#شامل <unistd.h>
#شامل <string.h>
#شامل <stdlib.h>
#شامل <systemlib/err.h>

#شامل "tinybson.h"

#اگر 0
# define debug(fmt, دلائل...)		کرو { PX4_INFO("BSON: " fmt, ##args); } جب(0)
#ورنہ
# define debug(fmt, دلائل...)		کرو { } جب(0)
#اگر_ختم

#تعریف_کرو CODER_CHECK(_c)		کرو { اگر ((_c)->dead) { PX4_ERR("coder dead"); واپس -1; }} جب(0)
#تعریف_کرو CODER_KILL(_c, _reason)	کرو { PX4_ERR("killed: %s", _reason); (_c)->dead = سچ; واپس -1; } جب(0)

ساکن صحیح
read_x(bson_decoder_t decoder, خالی *p, size_t s)
{
	CODER_CHECK(decoder);

	اگر (decoder->fd > -1) {
		صحیح ret = ::اقرأ(decoder->fd, p, s);

		اگر (ret == s) {
			decoder->کل_decoded_سائز += ret;
			واپس 0;
		}

		واپس -1;
	}

	اگر (decoder->buf != خالی_اشاریہ) {
		/* staged operations to avoid integer overflow for corrupt data */
		اگر (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		اگر ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		حافظہ_نقل(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->کل_decoded_سائز += s;
		واپس 0;
	}

	debug("no source");
	واپس -1;
}

ساکن صحیح
read_int8(bson_decoder_t decoder, int8_t *b)
{
	واپس read_x(decoder, b, حجم(*b));
}

ساکن صحیح
read_int32(bson_decoder_t decoder, int32_t *i)
{
	واپس read_x(decoder, i, حجم(*i));
}

ساکن صحیح
read_int64(bson_decoder_t decoder, int64_t *i)
{
	واپس read_x(decoder, i, حجم(*i));
}

ساکن صحیح
read_double(bson_decoder_t decoder, دوہرا *d)
{
	واپس read_x(decoder, d, حجم(*d));
}

صحیح
bson_decoder_init_file(bson_decoder_t decoder, صحیح fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->گرہ_2.type = BSON_UNDEFINED;

	// read document size
	اگر (read_int32(decoder, &decoder->کل_document_سائز)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->کل_document_سائز);

	// The length is signed and comes from the document itself
	اگر (decoder->کل_document_سائز < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	واپس 0;
}

صحیح
bson_decoder_init_buf(bson_decoder_t decoder, خالی *buf, غیرعلامتی bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	اگر ((buf == خالی_اشاریہ) || (callback == خالی_اشاریہ)) {
		واپس -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = جھوٹ;

	اگر (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} ورنہ {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->گرہ_2.type = BSON_UNDEFINED;
	decoder->کل_decoded_سائز = 0;

	// read document size
	decoder->کل_document_سائز = 0;

	اگر (read_int32(decoder, &decoder->کل_document_سائز)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->کل_document_سائز);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	اگر (decoder->کل_document_سائز < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	اگر ((decoder->کل_document_سائز > 0) && (decoder->کل_document_سائز > (صحیح)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	واپس 0;
}

صحیح
bson_decoder_اگلا(bson_decoder_t decoder)
{
	int8_t	tbyte;
	غیرعلامتی nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	اگر (decoder->گرہ_2.type == BSON_EOO) {
		اگر (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		اگر (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			واپس 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	جب (decoder->pending > 0) {
		اگر (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	اگر (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->گرہ_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->گرہ_2.type);

	/* EOO is special; it has no name/data following */
	اگر (decoder->گرہ_2.type == BSON_EOO) {
		decoder->گرہ_2.name[0] = '\0';

	} ورنہ اگر ((صحیح)decoder->گرہ_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = سچ;
		واپس -ENODATA;

	} ورنہ {

		/* get the node name */
		nlen = 0;

		برائے (;;) {
			اگر (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->گرہ_2.type, decoder->گرہ_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			اگر (read_int8(decoder, (int8_t *)&decoder->گرہ_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			اگر (decoder->گرہ_2.name[nlen] == '\0') {
				توڑو;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->گرہ_2.name);

		انتخاب (decoder->گرہ_2.type) {
		حالت BSON_DOUBLE:
			اگر (read_double(decoder, &decoder->گرہ_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->گنتی_گرہ_double++;
			توڑو;

		حالت BSON_STRING:
			اگر (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->گنتی_گرہ_string++;
			توڑو;

		حالت BSON_BINDATA:
			اگر (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			اگر (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->گرہ_2.subtype = (bson_ثنائی_subtype_t)tbyte;
			decoder->گنتی_گرہ_bindata++;
			توڑو;

		حالت BSON_BOOL:
			اگر (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->گرہ_2.b = (tbyte != 0);
			decoder->گنتی_گرہ_bool++;
			توڑو;

		حالت BSON_INT32:
			اگر (read_int32(decoder, &decoder->گرہ_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->گنتی_گرہ_int32++;
			توڑو;

		حالت BSON_INT64:
			اگر (read_int64(decoder, &decoder->گرہ_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->گنتی_گرہ_int64++;
			توڑو;

		حالت BSON_nullptr:
		حالت BSON_UNDEFINED:
			توڑو;

		/* XXX currently not supporting other types */
		طےشدہ:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	واپس decoder->callback(decoder, &decoder->گرہ_2);
}

صحیح
bson_decoder_copy_ڈیٹا(bson_decoder_t decoder, خالی *buf)
{
	صحیح نتیجہ;

	CODER_CHECK(decoder);

	/* copy data */
	نتیجہ = read_x(decoder, buf, decoder->pending);

	اگر (نتیجہ != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	واپس 0;
}

size_t
bson_decoder_ڈیٹا_pending(bson_decoder_t decoder)
{
	واپس decoder->pending;
}

ساکن صحیح
write_x(bson_encoder_t encoder, ثابت خالی *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	اگر (encoder->fd > -1 && encoder->buf == خالی_اشاریہ) {
		صحیح ret = ::اكتب(encoder->fd, p, s);

		اگر (ret == s) {
			encoder->کل_document_سائز += ret;
			واپس 0;
		}

		واپس -1;
	}

	/* do we need to extend the buffer? */
	جب ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		اگر (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			صحیح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

			اگر (ret == (صحیح)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->کل_document_سائز += ret;

				اگر ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				توڑو;

			} ورنہ {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		اگر (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		اگر (newbuf == خالی_اشاریہ) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	حافظہ_نقل(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	واپس 0;
}

ساکن صحیح
write_int8(bson_encoder_t encoder, int8_t b)
{
	واپس write_x(encoder, &b, حجم(b));
}

ساکن صحیح
write_int32(bson_encoder_t encoder, int32_t i)
{
	واپس write_x(encoder, &i, حجم(i));
}

ساکن صحیح
write_int64(bson_encoder_t encoder, int64_t i)
{
	واپس write_x(encoder, &i, حجم(i));
}

ساکن صحیح
write_double(bson_encoder_t encoder, دوہرا d)
{
	واپس write_x(encoder, &d, حجم(d));
}

ساکن صحیح
write_name(bson_encoder_t encoder, ثابت حرف *name)
{
	size_t len = لڑی_لمبائی(name);

	اگر (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	واپس write_x(encoder, name, len + 1);
}

صحیح
bson_encoder_init_file(bson_encoder_t encoder, صحیح fd)
{
	encoder->fd = fd;
	encoder->buf = خالی_اشاریہ;
	encoder->dead = جھوٹ;
	encoder->کل_document_سائز = 0;

	اگر (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	واپس 0;
}

صحیح
bson_encoder_init_buf_file(bson_encoder_t encoder, صحیح fd, خالی *buf, غیرعلامتی bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = جھوٹ;
	encoder->realloc_ok = جھوٹ;
	encoder->کل_document_سائز = 0;

	اگر (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	واپس 0;
}

صحیح
bson_encoder_init_buf(bson_encoder_t encoder, خالی *buf, غیرعلامتی bufsize)
{
	encoder->buf = (uint8_t *)buf;

	اگر (encoder->buf == خالی_اشاریہ) {
		encoder->realloc_ok = سچ;

	} ورنہ {
		encoder->bufsize = bufsize;
	}

	اگر (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	واپس 0;
}

صحیح
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	اگر (encoder->fd > -1 && encoder->buf != خالی_اشاریہ && encoder->bufpos > 0) {
		/* write final buffer to disk */
		صحیح ret = ::اكتب(encoder->fd, encoder->buf, encoder->bufpos);

		اگر (ret == (صحیح)encoder->bufpos) {
			encoder->کل_document_سائز += ret;

		} ورنہ {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ثابت int32_t bson_doc_بائٹیں = (encoder->fd > -1) ? encoder->کل_document_سائز : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_بائٹیں);

	اگر (encoder->fd > -1) {
		اگر ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::اكتب(encoder->fd, &bson_doc_بائٹیں, حجم(bson_doc_بائٹیں)) != حجم(bson_doc_بائٹیں))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} ورنہ اگر (encoder->buf != خالی_اشاریہ) {
		/* update buffer length */
		حافظہ_نقل(encoder->buf, &bson_doc_بائٹیں, حجم(bson_doc_بائٹیں));
	}

	واپس 0;
}

صحیح
bson_encoder_buf_سائز(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	اگر (encoder->fd > -1) {
		واپس -1;
	}

	واپس encoder->bufpos;
}

خالی *
bson_encoder_buf_ڈیٹا(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	اگر (encoder->fd > -1) {
		واپس خالی_اشاریہ;
	}

	واپس encoder->buf;
}

صحیح bson_encoder_append_bool(bson_encoder_t encoder, ثابت حرف *name, منطقی value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	واپس 0;
}

صحیح
bson_encoder_append_int32(bson_encoder_t encoder, ثابت حرف *name, int32_t value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	واپس 0;
}

صحیح
bson_encoder_append_int64(bson_encoder_t encoder, ثابت حرف *name, int64_t value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	واپس 0;
}

صحیح
bson_encoder_append_double(bson_encoder_t encoder, ثابت حرف *name, دوہرا value)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	واپس 0;
}

صحیح
bson_encoder_append_string(bson_encoder_t encoder, ثابت حرف *name, ثابت حرف *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = لڑی_لمبائی(string) + 1;	/* include trailing nul */

	اگر (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	واپس 0;
}

صحیح
bson_encoder_append_ثنائی(bson_encoder_t encoder, ثابت حرف *name, bson_ثنائی_subtype_t subtype, size_t سائز,
			   ثابت خالی *ڈیٹا)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, سائز) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, ڈیٹا, سائز)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	واپس 0;
}

صحیح
bson_encoder_append_null(bson_encoder_t encoder, ثابت حرف *name)
{
	CODER_CHECK(encoder);

	اگر (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	واپس 0;
}
