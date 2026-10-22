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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/posix.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <systemlib/err.h>

#ಅನ್ತರ್ಭಾವ "tinybson.h"

#ಆದರೆ 0
# define debug(fmt, ವಾದಗಳು...)		ಮಾಡು { PX4_INFO("BSON: " fmt, ##args); } ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
# define debug(fmt, ವಾದಗಳು...)		ಮಾಡು { } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CODER_CHECK(_c)		ಮಾಡು { ಆದರೆ ((_c)->dead) { PX4_ERR("coder dead"); ಹಿಂತಿರುಗಿ -1; }} ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CODER_KILL(_c, _reason)	ಮಾಡು { PX4_ERR("killed: %s", _reason); (_c)->dead = ನಿಜ; ಹಿಂತಿರುಗಿ -1; } ಆಗ(0)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_x(bson_decoder_t decoder, ಶೂನ್ಯ *p, size_t s)
{
	CODER_CHECK(decoder);

	ಆದರೆ (decoder->fd > -1) {
		ಪೂರ್ಣಾಂಕ ret = ::ಪಠ(decoder->fd, p, s);

		ಆದರೆ (ret == s) {
			decoder->ಒಟ್ಟು_decoded_ಗಾತ್ರ += ret;
			ಹಿಂತಿರುಗಿ 0;
		}

		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (decoder->buf != ಶೂನ್ಯಸೂಚಕ) {
		/* staged operations to avoid integer overflow for corrupt data */
		ಆದರೆ (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ಆದರೆ ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		ಸ್ಮರಣೆ_ನಕಲು(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->ಒಟ್ಟು_decoded_ಗಾತ್ರ += s;
		ಹಿಂತಿರುಗಿ 0;
	}

	debug("no source");
	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ಹಿಂತಿರುಗಿ read_x(decoder, b, ಗಾತ್ರ(*b));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ಹಿಂತಿರುಗಿ read_x(decoder, i, ಗಾತ್ರ(*i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ಹಿಂತಿರುಗಿ read_x(decoder, i, ಗಾತ್ರ(*i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_double(bson_decoder_t decoder, ದ್ವಿಗುಣ *d)
{
	ಹಿಂತಿರುಗಿ read_x(decoder, d, ಗಾತ್ರ(*d));
}

ಪೂರ್ಣಾಂಕ
bson_decoder_init_file(bson_decoder_t decoder, ಪೂರ್ಣಾಂಕ fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->ಗಂಟು_2.type = BSON_UNDEFINED;

	// read document size
	ಆದರೆ (read_int32(decoder, &decoder->ಒಟ್ಟು_document_ಗಾತ್ರ)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->ಒಟ್ಟು_document_ಗಾತ್ರ);

	// The length is signed and comes from the document itself
	ಆದರೆ (decoder->ಒಟ್ಟು_document_ಗಾತ್ರ < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_decoder_init_buf(bson_decoder_t decoder, ಶೂನ್ಯ *buf, ಚಿಹ್ನೆರಹಿತ bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ಆದರೆ ((buf == ಶೂನ್ಯಸೂಚಕ) || (callback == ಶೂನ್ಯಸೂಚಕ)) {
		ಹಿಂತಿರುಗಿ -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = ಸುಳ್ಳು;

	ಆದರೆ (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} ಇಲ್ಲದಿದ್ದರೆ {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->ಗಂಟು_2.type = BSON_UNDEFINED;
	decoder->ಒಟ್ಟು_decoded_ಗಾತ್ರ = 0;

	// read document size
	decoder->ಒಟ್ಟು_document_ಗಾತ್ರ = 0;

	ಆದರೆ (read_int32(decoder, &decoder->ಒಟ್ಟು_document_ಗಾತ್ರ)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->ಒಟ್ಟು_document_ಗಾತ್ರ);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ಆದರೆ (decoder->ಒಟ್ಟು_document_ಗಾತ್ರ < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ಆದರೆ ((decoder->ಒಟ್ಟು_document_ಗಾತ್ರ > 0) && (decoder->ಒಟ್ಟು_document_ಗಾತ್ರ > (ಪೂರ್ಣಾಂಕ)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_decoder_ಮುಂದಿನ(bson_decoder_t decoder)
{
	int8_t	tbyte;
	ಚಿಹ್ನೆರಹಿತ nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ಆದರೆ (decoder->ಗಂಟು_2.type == BSON_EOO) {
		ಆದರೆ (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ಆದರೆ (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ಹಿಂತಿರುಗಿ 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	ಆಗ (decoder->pending > 0) {
		ಆದರೆ (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ಆದರೆ (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->ಗಂಟು_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->ಗಂಟು_2.type);

	/* EOO is special; it has no name/data following */
	ಆದರೆ (decoder->ಗಂಟು_2.type == BSON_EOO) {
		decoder->ಗಂಟು_2.name[0] = '\0';

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಪೂರ್ಣಾಂಕ)decoder->ಗಂಟು_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = ನಿಜ;
		ಹಿಂತಿರುಗಿ -ENODATA;

	} ಇಲ್ಲದಿದ್ದರೆ {

		/* get the node name */
		nlen = 0;

		ಗಾಗಿ (;;) {
			ಆದರೆ (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->ಗಂಟು_2.type, decoder->ಗಂಟು_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ಆದರೆ (read_int8(decoder, (int8_t *)&decoder->ಗಂಟು_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ಆದರೆ (decoder->ಗಂಟು_2.name[nlen] == '\0') {
				ನಿಲ್ಲಿಸು;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->ಗಂಟು_2.name);

		ಆಯ್ಕೆ (decoder->ಗಂಟು_2.type) {
		ಸ್ಥಿತಿ BSON_DOUBLE:
			ಆದರೆ (read_double(decoder, &decoder->ಗಂಟು_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_double++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ BSON_STRING:
			ಆದರೆ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_string++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ BSON_BINDATA:
			ಆದರೆ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ಆದರೆ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->ಗಂಟು_2.subtype = (bson_ದ್ವಿಮಾನ_subtype_t)tbyte;
			decoder->ಎಣಿಕೆ_ಗಂಟು_bindata++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ BSON_BOOL:
			ಆದರೆ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->ಗಂಟು_2.b = (tbyte != 0);
			decoder->ಎಣಿಕೆ_ಗಂಟು_bool++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ BSON_INT32:
			ಆದರೆ (read_int32(decoder, &decoder->ಗಂಟು_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_int32++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ BSON_INT64:
			ಆದರೆ (read_int64(decoder, &decoder->ಗಂಟು_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_int64++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ BSON_nullptr:
		ಸ್ಥಿತಿ BSON_UNDEFINED:
			ನಿಲ್ಲಿಸು;

		/* XXX currently not supporting other types */
		ಪೂರ್ವನಿಯೋಜಿತ:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	ಹಿಂತಿರುಗಿ decoder->callback(decoder, &decoder->ಗಂಟು_2);
}

ಪೂರ್ಣಾಂಕ
bson_decoder_copy_ದತ್ತಾಂಶ(bson_decoder_t decoder, ಶೂನ್ಯ *buf)
{
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;

	CODER_CHECK(decoder);

	/* copy data */
	ಫಲಿತಾಂಶ = read_x(decoder, buf, decoder->pending);

	ಆದರೆ (ಫಲಿತಾಂಶ != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ಹಿಂತಿರುಗಿ 0;
}

size_t
bson_decoder_ದತ್ತಾಂಶ_pending(bson_decoder_t decoder)
{
	ಹಿಂತಿರುಗಿ decoder->pending;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_x(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ಆದರೆ (encoder->fd > -1 && encoder->buf == ಶೂನ್ಯಸೂಚಕ) {
		ಪೂರ್ಣಾಂಕ ret = ::ಲಿಖ(encoder->fd, p, s);

		ಆದರೆ (ret == s) {
			encoder->ಒಟ್ಟು_document_ಗಾತ್ರ += ret;
			ಹಿಂತಿರುಗಿ 0;
		}

		ಹಿಂತಿರುಗಿ -1;
	}

	/* do we need to extend the buffer? */
	ಆಗ ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ಆದರೆ (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			ಪೂರ್ಣಾಂಕ ret = ::ಲಿಖ(encoder->fd, encoder->buf, encoder->bufpos);

			ಆದರೆ (ret == (ಪೂರ್ಣಾಂಕ)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->ಒಟ್ಟು_document_ಗಾತ್ರ += ret;

				ಆದರೆ ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				ನಿಲ್ಲಿಸು;

			} ಇಲ್ಲದಿದ್ದರೆ {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ಆದರೆ (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ಆದರೆ (newbuf == ಶೂನ್ಯಸೂಚಕ) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	ಸ್ಮರಣೆ_ನಕಲು(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_int8(bson_encoder_t encoder, int8_t b)
{
	ಹಿಂತಿರುಗಿ write_x(encoder, &b, ಗಾತ್ರ(b));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_int32(bson_encoder_t encoder, int32_t i)
{
	ಹಿಂತಿರುಗಿ write_x(encoder, &i, ಗಾತ್ರ(i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_int64(bson_encoder_t encoder, int64_t i)
{
	ಹಿಂತಿರುಗಿ write_x(encoder, &i, ಗಾತ್ರ(i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_double(bson_encoder_t encoder, ದ್ವಿಗುಣ d)
{
	ಹಿಂತಿರುಗಿ write_x(encoder, &d, ಗಾತ್ರ(d));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
write_name(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);

	ಆದರೆ (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ಹಿಂತಿರುಗಿ write_x(encoder, name, len + 1);
}

ಪೂರ್ಣಾಂಕ
bson_encoder_init_file(bson_encoder_t encoder, ಪೂರ್ಣಾಂಕ fd)
{
	encoder->fd = fd;
	encoder->buf = ಶೂನ್ಯಸೂಚಕ;
	encoder->dead = ಸುಳ್ಳು;
	encoder->ಒಟ್ಟು_document_ಗಾತ್ರ = 0;

	ಆದರೆ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_init_buf_file(bson_encoder_t encoder, ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, ಚಿಹ್ನೆರಹಿತ bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = ಸುಳ್ಳು;
	encoder->realloc_ok = ಸುಳ್ಳು;
	encoder->ಒಟ್ಟು_document_ಗಾತ್ರ = 0;

	ಆದರೆ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_init_buf(bson_encoder_t encoder, ಶೂನ್ಯ *buf, ಚಿಹ್ನೆರಹಿತ bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ಆದರೆ (encoder->buf == ಶೂನ್ಯಸೂಚಕ) {
		encoder->realloc_ok = ನಿಜ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		encoder->bufsize = bufsize;
	}

	ಆದರೆ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ಆದರೆ (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ಆದರೆ (encoder->fd > -1 && encoder->buf != ಶೂನ್ಯಸೂಚಕ && encoder->bufpos > 0) {
		/* write final buffer to disk */
		ಪೂರ್ಣಾಂಕ ret = ::ಲಿಖ(encoder->fd, encoder->buf, encoder->bufpos);

		ಆದರೆ (ret == (ಪೂರ್ಣಾಂಕ)encoder->bufpos) {
			encoder->ಒಟ್ಟು_document_ಗಾತ್ರ += ret;

		} ಇಲ್ಲದಿದ್ದರೆ {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ಸ್ಥಿರಾಂಕ int32_t bson_doc_ಬೈಟುಗಳು = (encoder->fd > -1) ? encoder->ಒಟ್ಟು_document_ಗಾತ್ರ : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_ಬೈಟುಗಳು);

	ಆದರೆ (encoder->fd > -1) {
		ಆದರೆ ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::ಲಿಖ(encoder->fd, &bson_doc_ಬೈಟುಗಳು, ಗಾತ್ರ(bson_doc_ಬೈಟುಗಳು)) != ಗಾತ್ರ(bson_doc_ಬೈಟುಗಳು))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (encoder->buf != ಶೂನ್ಯಸೂಚಕ) {
		/* update buffer length */
		ಸ್ಮರಣೆ_ನಕಲು(encoder->buf, &bson_doc_ಬೈಟುಗಳು, ಗಾತ್ರ(bson_doc_ಬೈಟುಗಳು));
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_buf_ಗಾತ್ರ(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ಆದರೆ (encoder->fd > -1) {
		ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ encoder->bufpos;
}

ಶೂನ್ಯ *
bson_encoder_buf_ದತ್ತಾಂಶ(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ಆದರೆ (encoder->fd > -1) {
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
	}

	ಹಿಂತಿರುಗಿ encoder->buf;
}

ಪೂರ್ಣಾಂಕ bson_encoder_append_bool(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ತಾರ್ಕಿಕ value)
{
	CODER_CHECK(encoder);

	ಆದರೆ (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_append_int32(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, int32_t value)
{
	CODER_CHECK(encoder);

	ಆದರೆ (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_append_int64(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, int64_t value)
{
	CODER_CHECK(encoder);

	ಆದರೆ (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_append_double(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ದ್ವಿಗುಣ value)
{
	CODER_CHECK(encoder);

	ಆದರೆ (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_append_string(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string) + 1;	/* include trailing nul */

	ಆದರೆ (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_append_ದ್ವಿಮಾನ(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, bson_ದ್ವಿಮಾನ_subtype_t subtype, size_t ಗಾತ್ರ_2,
			   ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
	CODER_CHECK(encoder);

	ಆದರೆ (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, ಗಾತ್ರ_2) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, ದತ್ತಾಂಶ, ಗಾತ್ರ_2)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
bson_encoder_append_null(bson_encoder_t encoder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	CODER_CHECK(encoder);

	ಆದರೆ (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ಹಿಂತಿರುಗಿ 0;
}
