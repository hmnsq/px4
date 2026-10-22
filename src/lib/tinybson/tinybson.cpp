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

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <systemlib/err.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "tinybson.h"

#ਜੇ 0
# define debug(fmt, ਦਲੀਲਾਂ...)		ਕਰੋ { PX4_INFO("BSON: " fmt, ##args); } ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
# define debug(fmt, ਦਲੀਲਾਂ...)		ਕਰੋ { } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CODER_CHECK(_c)		ਕਰੋ { ਜੇ ((_c)->dead) { PX4_ERR("coder dead"); ਵਾਪਸ -1; }} ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CODER_KILL(_c, _reason)	ਕਰੋ { PX4_ERR("killed: %s", _reason); (_c)->dead = ਸੱਚ; ਵਾਪਸ -1; } ਜਦੋਂ(0)

ਸਥਿਰ ਪੂਰਨ
read_x(bson_decoder_t decoder, ਖਾਲੀ *p, size_t s)
{
	CODER_CHECK(decoder);

	ਜੇ (decoder->fd > -1) {
		ਪੂਰਨ ret = ::ਪਠ(decoder->fd, p, s);

		ਜੇ (ret == s) {
			decoder->ਕੁੱਲ_decoded_ਮਾਪ += ret;
			ਵਾਪਸ 0;
		}

		ਵਾਪਸ -1;
	}

	ਜੇ (decoder->buf != ਖਾਲੀ_ਸੂਚਕ) {
		/* staged operations to avoid integer overflow for corrupt data */
		ਜੇ (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ਜੇ ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		ਸਮ੍ਰਿਤੀ_ਨਕਲ(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->ਕੁੱਲ_decoded_ਮਾਪ += s;
		ਵਾਪਸ 0;
	}

	debug("no source");
	ਵਾਪਸ -1;
}

ਸਥਿਰ ਪੂਰਨ
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ਵਾਪਸ read_x(decoder, b, ਆਕਾਰ(*b));
}

ਸਥਿਰ ਪੂਰਨ
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ਵਾਪਸ read_x(decoder, i, ਆਕਾਰ(*i));
}

ਸਥਿਰ ਪੂਰਨ
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ਵਾਪਸ read_x(decoder, i, ਆਕਾਰ(*i));
}

ਸਥਿਰ ਪੂਰਨ
read_double(bson_decoder_t decoder, ਦੁੱਗਣਾ *d)
{
	ਵਾਪਸ read_x(decoder, d, ਆਕਾਰ(*d));
}

ਪੂਰਨ
bson_decoder_init_file(bson_decoder_t decoder, ਪੂਰਨ fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->ਗੰਢ_2.type = BSON_UNDEFINED;

	// read document size
	ਜੇ (read_int32(decoder, &decoder->ਕੁੱਲ_document_ਮਾਪ)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->ਕੁੱਲ_document_ਮਾਪ);

	// The length is signed and comes from the document itself
	ਜੇ (decoder->ਕੁੱਲ_document_ਮਾਪ < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_decoder_init_buf(bson_decoder_t decoder, ਖਾਲੀ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ਜੇ ((buf == ਖਾਲੀ_ਸੂਚਕ) || (callback == ਖਾਲੀ_ਸੂਚਕ)) {
		ਵਾਪਸ -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = ਝੂਠ;

	ਜੇ (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} ਨਹੀਂਤਾਂ {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->ਗੰਢ_2.type = BSON_UNDEFINED;
	decoder->ਕੁੱਲ_decoded_ਮਾਪ = 0;

	// read document size
	decoder->ਕੁੱਲ_document_ਮਾਪ = 0;

	ਜੇ (read_int32(decoder, &decoder->ਕੁੱਲ_document_ਮਾਪ)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->ਕੁੱਲ_document_ਮਾਪ);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ਜੇ (decoder->ਕੁੱਲ_document_ਮਾਪ < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ਜੇ ((decoder->ਕੁੱਲ_document_ਮਾਪ > 0) && (decoder->ਕੁੱਲ_document_ਮਾਪ > (ਪੂਰਨ)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_decoder_ਅਗਲਾ(bson_decoder_t decoder)
{
	int8_t	tbyte;
	ਬਿਨਾਨਿਸ਼ਾਨੀ nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ਜੇ (decoder->ਗੰਢ_2.type == BSON_EOO) {
		ਜੇ (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ਜੇ (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ਵਾਪਸ 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	ਜਦੋਂ (decoder->pending > 0) {
		ਜੇ (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ਜੇ (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->ਗੰਢ_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->ਗੰਢ_2.type);

	/* EOO is special; it has no name/data following */
	ਜੇ (decoder->ਗੰਢ_2.type == BSON_EOO) {
		decoder->ਗੰਢ_2.name[0] = '\0';

	} ਨਹੀਂਤਾਂ ਜੇ ((ਪੂਰਨ)decoder->ਗੰਢ_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = ਸੱਚ;
		ਵਾਪਸ -ENODATA;

	} ਨਹੀਂਤਾਂ {

		/* get the node name */
		nlen = 0;

		ਲਈ (;;) {
			ਜੇ (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->ਗੰਢ_2.type, decoder->ਗੰਢ_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ਜੇ (read_int8(decoder, (int8_t *)&decoder->ਗੰਢ_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ਜੇ (decoder->ਗੰਢ_2.name[nlen] == '\0') {
				ਰੋਕੋ;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->ਗੰਢ_2.name);

		ਚੋਣ (decoder->ਗੰਢ_2.type) {
		ਹਾਲਤ BSON_DOUBLE:
			ਜੇ (read_double(decoder, &decoder->ਗੰਢ_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->ਗਿਣਤੀ_ਗੰਢ_double++;
			ਰੋਕੋ;

		ਹਾਲਤ BSON_STRING:
			ਜੇ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->ਗਿਣਤੀ_ਗੰਢ_string++;
			ਰੋਕੋ;

		ਹਾਲਤ BSON_BINDATA:
			ਜੇ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ਜੇ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->ਗੰਢ_2.subtype = (bson_ਦੁਆਧਾਰੀ_subtype_t)tbyte;
			decoder->ਗਿਣਤੀ_ਗੰਢ_bindata++;
			ਰੋਕੋ;

		ਹਾਲਤ BSON_BOOL:
			ਜੇ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->ਗੰਢ_2.b = (tbyte != 0);
			decoder->ਗਿਣਤੀ_ਗੰਢ_bool++;
			ਰੋਕੋ;

		ਹਾਲਤ BSON_INT32:
			ਜੇ (read_int32(decoder, &decoder->ਗੰਢ_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ਗਿਣਤੀ_ਗੰਢ_int32++;
			ਰੋਕੋ;

		ਹਾਲਤ BSON_INT64:
			ਜੇ (read_int64(decoder, &decoder->ਗੰਢ_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ਗਿਣਤੀ_ਗੰਢ_int64++;
			ਰੋਕੋ;

		ਹਾਲਤ BSON_nullptr:
		ਹਾਲਤ BSON_UNDEFINED:
			ਰੋਕੋ;

		/* XXX currently not supporting other types */
		ਮੂਲ:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	ਵਾਪਸ decoder->callback(decoder, &decoder->ਗੰਢ_2);
}

ਪੂਰਨ
bson_decoder_copy_ਅੰਕੜੇ(bson_decoder_t decoder, ਖਾਲੀ *buf)
{
	ਪੂਰਨ ਨਤੀਜਾ;

	CODER_CHECK(decoder);

	/* copy data */
	ਨਤੀਜਾ = read_x(decoder, buf, decoder->pending);

	ਜੇ (ਨਤੀਜਾ != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ਵਾਪਸ 0;
}

size_t
bson_decoder_ਅੰਕੜੇ_pending(bson_decoder_t decoder)
{
	ਵਾਪਸ decoder->pending;
}

ਸਥਿਰ ਪੂਰਨ
write_x(bson_encoder_t encoder, ਅਚਲ ਖਾਲੀ *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ਜੇ (encoder->fd > -1 && encoder->buf == ਖਾਲੀ_ਸੂਚਕ) {
		ਪੂਰਨ ret = ::ਲਿਖ(encoder->fd, p, s);

		ਜੇ (ret == s) {
			encoder->ਕੁੱਲ_document_ਮਾਪ += ret;
			ਵਾਪਸ 0;
		}

		ਵਾਪਸ -1;
	}

	/* do we need to extend the buffer? */
	ਜਦੋਂ ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ਜੇ (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			ਪੂਰਨ ret = ::ਲਿਖ(encoder->fd, encoder->buf, encoder->bufpos);

			ਜੇ (ret == (ਪੂਰਨ)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->ਕੁੱਲ_document_ਮਾਪ += ret;

				ਜੇ ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				ਰੋਕੋ;

			} ਨਹੀਂਤਾਂ {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ਜੇ (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ਜੇ (newbuf == ਖਾਲੀ_ਸੂਚਕ) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ
write_int8(bson_encoder_t encoder, int8_t b)
{
	ਵਾਪਸ write_x(encoder, &b, ਆਕਾਰ(b));
}

ਸਥਿਰ ਪੂਰਨ
write_int32(bson_encoder_t encoder, int32_t i)
{
	ਵਾਪਸ write_x(encoder, &i, ਆਕਾਰ(i));
}

ਸਥਿਰ ਪੂਰਨ
write_int64(bson_encoder_t encoder, int64_t i)
{
	ਵਾਪਸ write_x(encoder, &i, ਆਕਾਰ(i));
}

ਸਥਿਰ ਪੂਰਨ
write_double(bson_encoder_t encoder, ਦੁੱਗਣਾ d)
{
	ਵਾਪਸ write_x(encoder, &d, ਆਕਾਰ(d));
}

ਸਥਿਰ ਪੂਰਨ
write_name(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name)
{
	size_t len = ਲੜੀ_ਲੰਬਾਈ(name);

	ਜੇ (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ਵਾਪਸ write_x(encoder, name, len + 1);
}

ਪੂਰਨ
bson_encoder_init_file(bson_encoder_t encoder, ਪੂਰਨ fd)
{
	encoder->fd = fd;
	encoder->buf = ਖਾਲੀ_ਸੂਚਕ;
	encoder->dead = ਝੂਠ;
	encoder->ਕੁੱਲ_document_ਮਾਪ = 0;

	ਜੇ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_init_buf_file(bson_encoder_t encoder, ਪੂਰਨ fd, ਖਾਲੀ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = ਝੂਠ;
	encoder->realloc_ok = ਝੂਠ;
	encoder->ਕੁੱਲ_document_ਮਾਪ = 0;

	ਜੇ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_init_buf(bson_encoder_t encoder, ਖਾਲੀ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ਜੇ (encoder->buf == ਖਾਲੀ_ਸੂਚਕ) {
		encoder->realloc_ok = ਸੱਚ;

	} ਨਹੀਂਤਾਂ {
		encoder->bufsize = bufsize;
	}

	ਜੇ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ਜੇ (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ਜੇ (encoder->fd > -1 && encoder->buf != ਖਾਲੀ_ਸੂਚਕ && encoder->bufpos > 0) {
		/* write final buffer to disk */
		ਪੂਰਨ ret = ::ਲਿਖ(encoder->fd, encoder->buf, encoder->bufpos);

		ਜੇ (ret == (ਪੂਰਨ)encoder->bufpos) {
			encoder->ਕੁੱਲ_document_ਮਾਪ += ret;

		} ਨਹੀਂਤਾਂ {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ਅਚਲ int32_t bson_doc_ਬਾਈਟਾਂ = (encoder->fd > -1) ? encoder->ਕੁੱਲ_document_ਮਾਪ : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_ਬਾਈਟਾਂ);

	ਜੇ (encoder->fd > -1) {
		ਜੇ ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::ਲਿਖ(encoder->fd, &bson_doc_ਬਾਈਟਾਂ, ਆਕਾਰ(bson_doc_ਬਾਈਟਾਂ)) != ਆਕਾਰ(bson_doc_ਬਾਈਟਾਂ))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} ਨਹੀਂਤਾਂ ਜੇ (encoder->buf != ਖਾਲੀ_ਸੂਚਕ) {
		/* update buffer length */
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(encoder->buf, &bson_doc_ਬਾਈਟਾਂ, ਆਕਾਰ(bson_doc_ਬਾਈਟਾਂ));
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_buf_ਮਾਪ(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ਜੇ (encoder->fd > -1) {
		ਵਾਪਸ -1;
	}

	ਵਾਪਸ encoder->bufpos;
}

ਖਾਲੀ *
bson_encoder_buf_ਅੰਕੜੇ(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ਜੇ (encoder->fd > -1) {
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
	}

	ਵਾਪਸ encoder->buf;
}

ਪੂਰਨ bson_encoder_append_bool(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, ਤਾਰਕਿਕ value)
{
	CODER_CHECK(encoder);

	ਜੇ (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_append_int32(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, int32_t value)
{
	CODER_CHECK(encoder);

	ਜੇ (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_append_int64(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, int64_t value)
{
	CODER_CHECK(encoder);

	ਜੇ (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_append_double(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, ਦੁੱਗਣਾ value)
{
	CODER_CHECK(encoder);

	ਜੇ (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_append_string(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = ਲੜੀ_ਲੰਬਾਈ(string) + 1;	/* include trailing nul */

	ਜੇ (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_append_ਦੁਆਧਾਰੀ(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name, bson_ਦੁਆਧਾਰੀ_subtype_t subtype, size_t ਮਾਪ,
			   ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
	CODER_CHECK(encoder);

	ਜੇ (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, ਮਾਪ) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, ਅੰਕੜੇ, ਮਾਪ)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
bson_encoder_append_null(bson_encoder_t encoder, ਅਚਲ ਅੱਖਰ *name)
{
	CODER_CHECK(encoder);

	ਜੇ (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ਵਾਪਸ 0;
}
