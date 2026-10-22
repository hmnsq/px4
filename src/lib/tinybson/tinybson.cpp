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

#அந்தர்பாவ <px4_platform_common/posix.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <systemlib/err.h>

#அந்தர்பாவ "tinybson.h"

#எனில் 0
# define debug(fmt, args...)		செய் { PX4_INFO("BSON: " fmt, ##args); } போது(0)
#இல்லையெனில்
# define debug(fmt, args...)		செய் { } போது(0)
#endif

#define CODER_CHECK(_c)		செய் { எனில் ((_c)->dead) { PX4_ERR("coder dead"); திரும்பு -1; }} போது(0)
#define CODER_KILL(_c, _reason)	செய் { PX4_ERR("killed: %s", _reason); (_c)->dead = உண்மை; திரும்பு -1; } போது(0)

நிலையான முழு
read_x(bson_decoder_t decoder, வெறுமை *p, size_t s)
{
	CODER_CHECK(decoder);

	எனில் (decoder->fd > -1) {
		முழு ret = ::பட(decoder->fd, p, s);

		எனில் (ret == s) {
			decoder->total_decoded_size += ret;
			திரும்பு 0;
		}

		திரும்பு -1;
	}

	எனில் (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		எனில் (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		எனில் ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		திரும்பு 0;
	}

	debug("no source");
	திரும்பு -1;
}

நிலையான முழு
read_int8(bson_decoder_t decoder, int8_t *b)
{
	திரும்பு read_x(decoder, b, அளவு(*b));
}

நிலையான முழு
read_int32(bson_decoder_t decoder, int32_t *i)
{
	திரும்பு read_x(decoder, i, அளவு(*i));
}

நிலையான முழு
read_int64(bson_decoder_t decoder, int64_t *i)
{
	திரும்பு read_x(decoder, i, அளவு(*i));
}

நிலையான முழு
read_double(bson_decoder_t decoder, இரட்டை *d)
{
	திரும்பு read_x(decoder, d, அளவு(*d));
}

முழு
bson_decoder_init_file(bson_decoder_t decoder, முழு fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->முனை_2.type = BSON_UNDEFINED;

	// read document size
	எனில் (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	எனில் (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	திரும்பு 0;
}

முழு
bson_decoder_init_buf(bson_decoder_t decoder, வெறுமை *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	எனில் ((buf == nullptr) || (callback == nullptr)) {
		திரும்பு -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = பொய்;

	எனில் (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} இல்லையெனில் {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->முனை_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	எனில் (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	எனில் (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	எனில் ((decoder->total_document_size > 0) && (decoder->total_document_size > (முழு)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	திரும்பு 0;
}

முழு
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	எனில் (decoder->முனை_2.type == BSON_EOO) {
		எனில் (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		எனில் (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			திரும்பு 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	போது (decoder->pending > 0) {
		எனில் (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	எனில் (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->முனை_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->முனை_2.type);

	/* EOO is special; it has no name/data following */
	எனில் (decoder->முனை_2.type == BSON_EOO) {
		decoder->முனை_2.name[0] = '\0';

	} இல்லையெனில் எனில் ((முழு)decoder->முனை_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = உண்மை;
		திரும்பு -ENODATA;

	} இல்லையெனில் {

		/* get the node name */
		nlen = 0;

		க்கு (;;) {
			எனில் (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->முனை_2.type, decoder->முனை_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			எனில் (read_int8(decoder, (int8_t *)&decoder->முனை_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			எனில் (decoder->முனை_2.name[nlen] == '\0') {
				நிறுத்து;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->முனை_2.name);

		தேர்வு (decoder->முனை_2.type) {
		நிலை BSON_DOUBLE:
			எனில் (read_double(decoder, &decoder->முனை_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->மொத்தம்_முனை_double++;
			நிறுத்து;

		நிலை BSON_STRING:
			எனில் (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->மொத்தம்_முனை_string++;
			நிறுத்து;

		நிலை BSON_BINDATA:
			எனில் (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			எனில் (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->முனை_2.subtype = (bson_இருமம்_subtype_t)tbyte;
			decoder->மொத்தம்_முனை_bindata++;
			நிறுத்து;

		நிலை BSON_BOOL:
			எனில் (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->முனை_2.b = (tbyte != 0);
			decoder->மொத்தம்_முனை_bool++;
			நிறுத்து;

		நிலை BSON_INT32:
			எனில் (read_int32(decoder, &decoder->முனை_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->மொத்தம்_முனை_int32++;
			நிறுத்து;

		நிலை BSON_INT64:
			எனில் (read_int64(decoder, &decoder->முனை_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->மொத்தம்_முனை_int64++;
			நிறுத்து;

		நிலை BSON_nullptr:
		நிலை BSON_UNDEFINED:
			நிறுத்து;

		/* XXX currently not supporting other types */
		இயல்பு:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	திரும்பு decoder->callback(decoder, &decoder->முனை_2);
}

முழு
bson_decoder_copy_data(bson_decoder_t decoder, வெறுமை *buf)
{
	முழு result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	எனில் (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	திரும்பு 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	திரும்பு decoder->pending;
}

நிலையான முழு
write_x(bson_encoder_t encoder, மாறிலி வெறுமை *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	எனில் (encoder->fd > -1 && encoder->buf == nullptr) {
		முழு ret = ::லிக(encoder->fd, p, s);

		எனில் (ret == s) {
			encoder->total_document_size += ret;
			திரும்பு 0;
		}

		திரும்பு -1;
	}

	/* do we need to extend the buffer? */
	போது ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		எனில் (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			முழு ret = ::லிக(encoder->fd, encoder->buf, encoder->bufpos);

			எனில் (ret == (முழு)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				எனில் ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				நிறுத்து;

			} இல்லையெனில் {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		எனில் (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		எனில் (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	திரும்பு 0;
}

நிலையான முழு
write_int8(bson_encoder_t encoder, int8_t b)
{
	திரும்பு write_x(encoder, &b, அளவு(b));
}

நிலையான முழு
write_int32(bson_encoder_t encoder, int32_t i)
{
	திரும்பு write_x(encoder, &i, அளவு(i));
}

நிலையான முழு
write_int64(bson_encoder_t encoder, int64_t i)
{
	திரும்பு write_x(encoder, &i, அளவு(i));
}

நிலையான முழு
write_double(bson_encoder_t encoder, இரட்டை d)
{
	திரும்பு write_x(encoder, &d, அளவு(d));
}

நிலையான முழு
write_name(bson_encoder_t encoder, மாறிலி எழுத்து *name)
{
	size_t len = strlen(name);

	எனில் (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	திரும்பு write_x(encoder, name, len + 1);
}

முழு
bson_encoder_init_file(bson_encoder_t encoder, முழு fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = பொய்;
	encoder->total_document_size = 0;

	எனில் (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	திரும்பு 0;
}

முழு
bson_encoder_init_buf_file(bson_encoder_t encoder, முழு fd, வெறுமை *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = பொய்;
	encoder->realloc_ok = பொய்;
	encoder->total_document_size = 0;

	எனில் (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	திரும்பு 0;
}

முழு
bson_encoder_init_buf(bson_encoder_t encoder, வெறுமை *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	எனில் (encoder->buf == nullptr) {
		encoder->realloc_ok = உண்மை;

	} இல்லையெனில் {
		encoder->bufsize = bufsize;
	}

	எனில் (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	திரும்பு 0;
}

முழு
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	எனில் (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	எனில் (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		முழு ret = ::லிக(encoder->fd, encoder->buf, encoder->bufpos);

		எனில் (ret == (முழு)encoder->bufpos) {
			encoder->total_document_size += ret;

		} இல்லையெனில் {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	மாறிலி int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	எனில் (encoder->fd > -1) {
		எனில் ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::லிக(encoder->fd, &bson_doc_bytes, அளவு(bson_doc_bytes)) != அளவு(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} இல்லையெனில் எனில் (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, அளவு(bson_doc_bytes));
	}

	திரும்பு 0;
}

முழு
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	எனில் (encoder->fd > -1) {
		திரும்பு -1;
	}

	திரும்பு encoder->bufpos;
}

வெறுமை *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	எனில் (encoder->fd > -1) {
		திரும்பு nullptr;
	}

	திரும்பு encoder->buf;
}

முழு bson_encoder_append_bool(bson_encoder_t encoder, மாறிலி எழுத்து *name, தருக்க value)
{
	CODER_CHECK(encoder);

	எனில் (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	திரும்பு 0;
}

முழு
bson_encoder_append_int32(bson_encoder_t encoder, மாறிலி எழுத்து *name, int32_t value)
{
	CODER_CHECK(encoder);

	எனில் (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	திரும்பு 0;
}

முழு
bson_encoder_append_int64(bson_encoder_t encoder, மாறிலி எழுத்து *name, int64_t value)
{
	CODER_CHECK(encoder);

	எனில் (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	திரும்பு 0;
}

முழு
bson_encoder_append_double(bson_encoder_t encoder, மாறிலி எழுத்து *name, இரட்டை value)
{
	CODER_CHECK(encoder);

	எனில் (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	திரும்பு 0;
}

முழு
bson_encoder_append_string(bson_encoder_t encoder, மாறிலி எழுத்து *name, மாறிலி எழுத்து *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	எனில் (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	திரும்பு 0;
}

முழு
bson_encoder_append_இருமம்(bson_encoder_t encoder, மாறிலி எழுத்து *name, bson_இருமம்_subtype_t subtype, size_t size,
			   மாறிலி வெறுமை *data)
{
	CODER_CHECK(encoder);

	எனில் (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	திரும்பு 0;
}

முழு
bson_encoder_append_null(bson_encoder_t encoder, மாறிலி எழுத்து *name)
{
	CODER_CHECK(encoder);

	எனில் (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	திரும்பு 0;
}
