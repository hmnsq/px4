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

#ಯದಿ 0
# define debug(fmt, args...)		ಕುರು { PX4_INFO("BSON: " fmt, ##args); } ಯಾವತ್(0)
#ಅನ್ಯಥಾ
# define debug(fmt, args...)		ಕುರು { } ಯಾವತ್(0)
#endif

#define CODER_CHECK(_c)		ಕುರು { ಯದಿ ((_c)->dead) { PX4_ERR("coder dead"); ಪ್ರತ್ಯಾಗಮ -1; }} ಯಾವತ್(0)
#define CODER_KILL(_c, _reason)	ಕುರು { PX4_ERR("killed: %s", _reason); (_c)->dead = ಸತ್ಯ; ಪ್ರತ್ಯಾಗಮ -1; } ಯಾವತ್(0)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_x(bson_decoder_t decoder, ಶೂನ್ಯ *p, size_t s)
{
	CODER_CHECK(decoder);

	ಯದಿ (decoder->fd > -1) {
		ಪೂರ್ಣಾಙ್ಕ ret = ::ಪಠ(decoder->fd, p, s);

		ಯದಿ (ret == s) {
			decoder->total_decoded_size += ret;
			ಪ್ರತ್ಯಾಗಮ 0;
		}

		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		ಯದಿ (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		ಯದಿ ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	debug("no source");
	ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_int8(bson_decoder_t decoder, int8_t *b)
{
	ಪ್ರತ್ಯಾಗಮ read_x(decoder, b, ಪರಿಮಾಣ(*b));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_int32(bson_decoder_t decoder, int32_t *i)
{
	ಪ್ರತ್ಯಾಗಮ read_x(decoder, i, ಪರಿಮಾಣ(*i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_int64(bson_decoder_t decoder, int64_t *i)
{
	ಪ್ರತ್ಯಾಗಮ read_x(decoder, i, ಪರಿಮಾಣ(*i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_double(bson_decoder_t decoder, ದ್ವಿಗುಣ *d)
{
	ಪ್ರತ್ಯಾಗಮ read_x(decoder, d, ಪರಿಮಾಣ(*d));
}

ಪೂರ್ಣಾಙ್ಕ
bson_decoder_init_file(bson_decoder_t decoder, ಪೂರ್ಣಾಙ್ಕ fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->ಗಂಟು_2.type = BSON_UNDEFINED;

	// read document size
	ಯದಿ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	ಯದಿ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_decoder_init_buf(bson_decoder_t decoder, ಶೂನ್ಯ *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	ಯದಿ ((buf == nullptr) || (callback == nullptr)) {
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = ಅಸತ್ಯ;

	ಯದಿ (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} ಅನ್ಯಥಾ {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->ಗಂಟು_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	ಯದಿ (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	ಯದಿ (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	ಯದಿ ((decoder->total_document_size > 0) && (decoder->total_document_size > (ಪೂರ್ಣಾಙ್ಕ)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	ಯದಿ (decoder->ಗಂಟು_2.type == BSON_EOO) {
		ಯದಿ (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		ಯದಿ (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			ಪ್ರತ್ಯಾಗಮ 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	ಯಾವತ್ (decoder->pending > 0) {
		ಯದಿ (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	ಯದಿ (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->ಗಂಟು_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->ಗಂಟು_2.type);

	/* EOO is special; it has no name/data following */
	ಯದಿ (decoder->ಗಂಟು_2.type == BSON_EOO) {
		decoder->ಗಂಟು_2.name[0] = '\0';

	} ಅನ್ಯಥಾ ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)decoder->ಗಂಟು_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = ಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ -ENODATA;

	} ಅನ್ಯಥಾ {

		/* get the node name */
		nlen = 0;

		ಪ್ರತಿ (;;) {
			ಯದಿ (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->ಗಂಟು_2.type, decoder->ಗಂಟು_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			ಯದಿ (read_int8(decoder, (int8_t *)&decoder->ಗಂಟು_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			ಯದಿ (decoder->ಗಂಟು_2.name[nlen] == '\0') {
				ಭಙ್ಗ;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->ಗಂಟು_2.name);

		ವಿಕಲ್ಪ (decoder->ಗಂಟು_2.type) {
		ಪಕ್ಷ BSON_DOUBLE:
			ಯದಿ (read_double(decoder, &decoder->ಗಂಟು_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_double++;
			ಭಙ್ಗ;

		ಪಕ್ಷ BSON_STRING:
			ಯದಿ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_string++;
			ಭಙ್ಗ;

		ಪಕ್ಷ BSON_BINDATA:
			ಯದಿ (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			ಯದಿ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->ಗಂಟು_2.subtype = (bson_ದ್ವಿಮಾನ_subtype_t)tbyte;
			decoder->ಎಣಿಕೆ_ಗಂಟು_bindata++;
			ಭಙ್ಗ;

		ಪಕ್ಷ BSON_BOOL:
			ಯದಿ (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->ಗಂಟು_2.b = (tbyte != 0);
			decoder->ಎಣಿಕೆ_ಗಂಟು_bool++;
			ಭಙ್ಗ;

		ಪಕ್ಷ BSON_INT32:
			ಯದಿ (read_int32(decoder, &decoder->ಗಂಟು_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_int32++;
			ಭಙ್ಗ;

		ಪಕ್ಷ BSON_INT64:
			ಯದಿ (read_int64(decoder, &decoder->ಗಂಟು_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->ಎಣಿಕೆ_ಗಂಟು_int64++;
			ಭಙ್ಗ;

		ಪಕ್ಷ BSON_nullptr:
		ಪಕ್ಷ BSON_UNDEFINED:
			ಭಙ್ಗ;

		/* XXX currently not supporting other types */
		ಸ್ವಭಾವ:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	ಪ್ರತ್ಯಾಗಮ decoder->callback(decoder, &decoder->ಗಂಟು_2);
}

ಪೂರ್ಣಾಙ್ಕ
bson_decoder_copy_data(bson_decoder_t decoder, ಶೂನ್ಯ *buf)
{
	ಪೂರ್ಣಾಙ್ಕ result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	ಯದಿ (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	ಪ್ರತ್ಯಾಗಮ 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	ಪ್ರತ್ಯಾಗಮ decoder->pending;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_x(bson_encoder_t encoder, ಧ್ರುವ ಶೂನ್ಯ *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	ಯದಿ (encoder->fd > -1 && encoder->buf == nullptr) {
		ಪೂರ್ಣಾಙ್ಕ ret = ::ಲಿಖ(encoder->fd, p, s);

		ಯದಿ (ret == s) {
			encoder->total_document_size += ret;
			ಪ್ರತ್ಯಾಗಮ 0;
		}

		ಪ್ರತ್ಯಾಗಮ -1;
	}

	/* do we need to extend the buffer? */
	ಯಾವತ್ ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		ಯದಿ (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			ಪೂರ್ಣಾಙ್ಕ ret = ::ಲಿಖ(encoder->fd, encoder->buf, encoder->bufpos);

			ಯದಿ (ret == (ಪೂರ್ಣಾಙ್ಕ)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				ಯದಿ ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				ಭಙ್ಗ;

			} ಅನ್ಯಥಾ {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		ಯದಿ (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		ಯದಿ (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_int8(bson_encoder_t encoder, int8_t b)
{
	ಪ್ರತ್ಯಾಗಮ write_x(encoder, &b, ಪರಿಮಾಣ(b));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_int32(bson_encoder_t encoder, int32_t i)
{
	ಪ್ರತ್ಯಾಗಮ write_x(encoder, &i, ಪರಿಮಾಣ(i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_int64(bson_encoder_t encoder, int64_t i)
{
	ಪ್ರತ್ಯಾಗಮ write_x(encoder, &i, ಪರಿಮಾಣ(i));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_double(bson_encoder_t encoder, ದ್ವಿಗುಣ d)
{
	ಪ್ರತ್ಯಾಗಮ write_x(encoder, &d, ಪರಿಮಾಣ(d));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_name(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name)
{
	size_t len = strlen(name);

	ಯದಿ (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	ಪ್ರತ್ಯಾಗಮ write_x(encoder, name, len + 1);
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_init_file(bson_encoder_t encoder, ಪೂರ್ಣಾಙ್ಕ fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = ಅಸತ್ಯ;
	encoder->total_document_size = 0;

	ಯದಿ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_init_buf_file(bson_encoder_t encoder, ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = ಅಸತ್ಯ;
	encoder->realloc_ok = ಅಸತ್ಯ;
	encoder->total_document_size = 0;

	ಯದಿ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_init_buf(bson_encoder_t encoder, ಶೂನ್ಯ *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	ಯದಿ (encoder->buf == nullptr) {
		encoder->realloc_ok = ಸತ್ಯ;

	} ಅನ್ಯಥಾ {
		encoder->bufsize = bufsize;
	}

	ಯದಿ (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ಯದಿ (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	ಯದಿ (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		ಪೂರ್ಣಾಙ್ಕ ret = ::ಲಿಖ(encoder->fd, encoder->buf, encoder->bufpos);

		ಯದಿ (ret == (ಪೂರ್ಣಾಙ್ಕ)encoder->bufpos) {
			encoder->total_document_size += ret;

		} ಅನ್ಯಥಾ {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ಧ್ರುವ int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	ಯದಿ (encoder->fd > -1) {
		ಯದಿ ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::ಲಿಖ(encoder->fd, &bson_doc_bytes, ಪರಿಮಾಣ(bson_doc_bytes)) != ಪರಿಮಾಣ(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} ಅನ್ಯಥಾ ಯದಿ (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, ಪರಿಮಾಣ(bson_doc_bytes));
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	ಯದಿ (encoder->fd > -1) {
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ encoder->bufpos;
}

ಶೂನ್ಯ *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	ಯದಿ (encoder->fd > -1) {
		ಪ್ರತ್ಯಾಗಮ nullptr;
	}

	ಪ್ರತ್ಯಾಗಮ encoder->buf;
}

ಪೂರ್ಣಾಙ್ಕ bson_encoder_append_bool(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name, ತಾರ್ಕಿಕ value)
{
	CODER_CHECK(encoder);

	ಯದಿ (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_append_int32(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name, int32_t value)
{
	CODER_CHECK(encoder);

	ಯದಿ (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_append_int64(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name, int64_t value)
{
	CODER_CHECK(encoder);

	ಯದಿ (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_append_double(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name, ದ್ವಿಗುಣ value)
{
	CODER_CHECK(encoder);

	ಯದಿ (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_append_string(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	ಯದಿ (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_append_ದ್ವಿಮಾನ(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name, bson_ದ್ವಿಮಾನ_subtype_t subtype, size_t size,
			   ಧ್ರುವ ಶೂನ್ಯ *data)
{
	CODER_CHECK(encoder);

	ಯದಿ (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
bson_encoder_append_null(bson_encoder_t encoder, ಧ್ರುವ ವರ್ಣ *name)
{
	CODER_CHECK(encoder);

	ಯದಿ (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}
