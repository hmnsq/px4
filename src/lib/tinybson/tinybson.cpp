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

#अन्तर्भाव <px4_platform_common/posix.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <systemlib/err.h>

#अन्तर्भाव "tinybson.h"

#सचे 0
# define debug(fmt, args...)		करोहि { PX4_INFO("BSON: " fmt, ##args); } याव(0)
#अञ्ञथा
# define debug(fmt, args...)		करोहि { } याव(0)
#endif

#define CODER_CHECK(_c)		करोहि { सचे ((_c)->dead) { PX4_ERR("coder dead"); पटिनिवत्त -1; }} याव(0)
#define CODER_KILL(_c, _reason)	करोहि { PX4_ERR("killed: %s", _reason); (_c)->dead = सच्च; पटिनिवत्त -1; } याव(0)

स्थिर पूर्णाङ्क
read_x(bson_decoder_t decoder, शून्य *p, size_t s)
{
	CODER_CHECK(decoder);

	सचे (decoder->fd > -1) {
		पूर्णाङ्क ret = ::पठ(decoder->fd, p, s);

		सचे (ret == s) {
			decoder->total_decoded_size += ret;
			पटिनिवत्त 0;
		}

		पटिनिवत्त -1;
	}

	सचे (decoder->buf != nullptr) {
		/* staged operations to avoid integer overflow for corrupt data */
		सचे (s >= decoder->bufsize) {
			CODER_KILL(decoder, "buffer too small for read");
		}

		सचे ((decoder->bufsize - s) < decoder->bufpos) {
			CODER_KILL(decoder, "not enough data for read");
		}

		memcpy(p, (decoder->buf + decoder->bufpos), s);
		decoder->bufpos += s;
		decoder->total_decoded_size += s;
		पटिनिवत्त 0;
	}

	debug("no source");
	पटिनिवत्त -1;
}

स्थिर पूर्णाङ्क
read_int8(bson_decoder_t decoder, int8_t *b)
{
	पटिनिवत्त read_x(decoder, b, परिमाण(*b));
}

स्थिर पूर्णाङ्क
read_int32(bson_decoder_t decoder, int32_t *i)
{
	पटिनिवत्त read_x(decoder, i, परिमाण(*i));
}

स्थिर पूर्णाङ्क
read_int64(bson_decoder_t decoder, int64_t *i)
{
	पटिनिवत्त read_x(decoder, i, परिमाण(*i));
}

स्थिर पूर्णाङ्क
read_double(bson_decoder_t decoder, द्विगुण *d)
{
	पटिनिवत्त read_x(decoder, d, परिमाण(*d));
}

पूर्णाङ्क
bson_decoder_init_file(bson_decoder_t decoder, पूर्णाङ्क fd, bson_decoder_callback callback)
{
	decoder->fd = fd;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->ग्रन्थिः_2.type = BSON_UNDEFINED;

	// read document size
	सचे (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// The length is signed and comes from the document itself
	सचे (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	/* ready for decoding */
	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_decoder_init_buf(bson_decoder_t decoder, शून्य *buf, unsigned bufsize, bson_decoder_callback callback)
{
	/* argument sanity */
	सचे ((buf == nullptr) || (callback == nullptr)) {
		पटिनिवत्त -1;
	}

	decoder->fd = -1;
	decoder->buf = (uint8_t *)buf;
	decoder->dead = मिच्छा;

	सचे (bufsize == 0) {
		decoder->bufsize = *(uint32_t *)buf;
		debug("auto-detected %zu byte object", decoder->bufsize);

	} अञ्ञथा {
		decoder->bufsize = bufsize;
	}

	decoder->bufpos = 0;
	decoder->callback = callback;
	decoder->nesting = 1;
	decoder->pending = 0;
	decoder->ग्रन्थिः_2.type = BSON_UNDEFINED;
	decoder->total_decoded_size = 0;

	// read document size
	decoder->total_document_size = 0;

	सचे (read_int32(decoder, &decoder->total_document_size)) {
		CODER_KILL(decoder, "failed reading length");
	}

	debug("total document size = %" PRIi32, decoder->total_document_size);

	// A negative length would skip the size check below, which only rejects lengths above
	// the buffer
	सचे (decoder->total_document_size < 0) {
		CODER_KILL(decoder, "negative document length");
	}

	सचे ((decoder->total_document_size > 0) && (decoder->total_document_size > (पूर्णाङ्क)decoder->bufsize)) {
		CODER_KILL(decoder, "document length larger than buffer");
	}

	/* ready for decoding */
	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_decoder_next(bson_decoder_t decoder)
{
	int8_t	tbyte;
	unsigned nlen;

	CODER_CHECK(decoder);

	/* if the previous node was EOO, pop a nesting level */
	सचे (decoder->ग्रन्थिः_2.type == BSON_EOO) {
		सचे (decoder->nesting > 0) {
			decoder->nesting--;
		}

		/* if the nesting level is now zero, the top-level document is done */
		सचे (decoder->nesting == 0) {
			/* like kill but not an error */
			debug("nesting is zero, document is done");
			decoder->fd = -1;

			/* return end-of-file to the caller */
			पटिनिवत्त 0;
		}
	}

	/* if there are unread bytes pending in the stream, discard them */
	याव (decoder->pending > 0) {
		सचे (read_int8(decoder, &tbyte)) {
			CODER_KILL(decoder, "read error discarding pending bytes");
		}

		decoder->pending--;
	}

	/* get the type byte */
	सचे (read_int8(decoder, &tbyte)) {
		CODER_KILL(decoder, "read error on type byte");
	}

	decoder->ग्रन्थिः_2.type = (bson_type_t)tbyte;
	decoder->pending = 0;

	debug("got type byte 0x%02x", decoder->ग्रन्थिः_2.type);

	/* EOO is special; it has no name/data following */
	सचे (decoder->ग्रन्थिः_2.type == BSON_EOO) {
		decoder->ग्रन्थिः_2.name[0] = '\0';

	} अञ्ञथा सचे ((पूर्णाङ्क)decoder->ग्रन्थिः_2.type == 0xff) { // indicates erased FLASH
		decoder->dead = सच्च;
		पटिनिवत्त -ENODATA;

	} अञ्ञथा {

		/* get the node name */
		nlen = 0;

		अत्थाय (;;) {
			सचे (nlen >= BSON_MAXNAME) {
				PX4_ERR("node name overflow, type:0x%02x, name:%.32s", decoder->ग्रन्थिः_2.type, decoder->ग्रन्थिः_2.name);
				CODER_KILL(decoder, "node name overflow");
			}

			सचे (read_int8(decoder, (int8_t *)&decoder->ग्रन्थिः_2.name[nlen])) {
				CODER_KILL(decoder, "read error on node name");
			}

			सचे (decoder->ग्रन्थिः_2.name[nlen] == '\0') {
				भङ्ग;
			}

			nlen++;
		}

		debug("got name '%s'", decoder->ग्रन्थिः_2.name);

		विकल्प (decoder->ग्रन्थिः_2.type) {
		पक्ष BSON_DOUBLE:
			सचे (read_double(decoder, &decoder->ग्रन्थिः_2.d)) {
				CODER_KILL(decoder, "read error on BSON_DOUBLE");
			}

			decoder->गणनम्_ग्रन्थिः_double++;
			भङ्ग;

		पक्ष BSON_STRING:
			सचे (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_STRING length");
			}

			decoder->गणनम्_ग्रन्थिः_string++;
			भङ्ग;

		पक्ष BSON_BINDATA:
			सचे (read_int32(decoder, &decoder->pending)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA size");
			}

			सचे (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BINDATA subtype");
			}

			decoder->ग्रन्थिः_2.subtype = (bson_द्विमान_subtype_t)tbyte;
			decoder->गणनम्_ग्रन्थिः_bindata++;
			भङ्ग;

		पक्ष BSON_BOOL:
			सचे (read_int8(decoder, &tbyte)) {
				CODER_KILL(decoder, "read error on BSON_BOOL");
			}

			decoder->ग्रन्थिः_2.b = (tbyte != 0);
			decoder->गणनम्_ग्रन्थिः_bool++;
			भङ्ग;

		पक्ष BSON_INT32:
			सचे (read_int32(decoder, &decoder->ग्रन्थिः_2.i32)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->गणनम्_ग्रन्थिः_int32++;
			भङ्ग;

		पक्ष BSON_INT64:
			सचे (read_int64(decoder, &decoder->ग्रन्थिः_2.i64)) {
				CODER_KILL(decoder, "read error on BSON_INT");
			}

			decoder->गणनम्_ग्रन्थिः_int64++;
			भङ्ग;

		पक्ष BSON_nullptr:
		पक्ष BSON_UNDEFINED:
			भङ्ग;

		/* XXX currently not supporting other types */
		स्वभाव:
			CODER_KILL(decoder, "unsupported node type");
		}
	}

	/* call the callback and pass its results back */
	पटिनिवत्त decoder->callback(decoder, &decoder->ग्रन्थिः_2);
}

पूर्णाङ्क
bson_decoder_copy_data(bson_decoder_t decoder, शून्य *buf)
{
	पूर्णाङ्क result;

	CODER_CHECK(decoder);

	/* copy data */
	result = read_x(decoder, buf, decoder->pending);

	सचे (result != 0) {
		CODER_KILL(decoder, "read error on copy_data");
	}

	/* pending count is discharged */
	decoder->pending = 0;
	पटिनिवत्त 0;
}

size_t
bson_decoder_data_pending(bson_decoder_t decoder)
{
	पटिनिवत्त decoder->pending;
}

स्थिर पूर्णाङ्क
write_x(bson_encoder_t encoder, ध्रुव शून्य *p, size_t s)
{
	CODER_CHECK(encoder);

	/* bson file encoder (non-buffered) */
	सचे (encoder->fd > -1 && encoder->buf == nullptr) {
		पूर्णाङ्क ret = ::लिख(encoder->fd, p, s);

		सचे (ret == s) {
			encoder->total_document_size += ret;
			पटिनिवत्त 0;
		}

		पटिनिवत्त -1;
	}

	/* do we need to extend the buffer? */
	याव ((encoder->bufpos + s) > encoder->bufsize) {

		/* bson buffered file encoder */
		सचे (encoder->fd > -1) {
			// write to disk
			debug("writing buffer (%d) to disk", encoder->bufpos);
			पूर्णाङ्क ret = ::लिख(encoder->fd, encoder->buf, encoder->bufpos);

			सचे (ret == (पूर्णाङ्क)encoder->bufpos) {
				// reset buffer to beginning and continue
				encoder->bufpos = 0;
				encoder->total_document_size += ret;

				सचे ((encoder->bufpos + s) > encoder->bufsize) {
					CODER_KILL(encoder, "fixed-size buffer overflow");
				}

				भङ्ग;

			} अञ्ञथा {
				PX4_ERR("file write error %d, errno:%d (%s)", ret, errno, strerror(errno));
				CODER_KILL(encoder, "file write error");
			}
		}

		सचे (!encoder->realloc_ok) {
			CODER_KILL(encoder, "fixed-size buffer overflow");
		}

		uint8_t *newbuf = (uint8_t *)realloc(encoder->buf, encoder->bufsize + BSON_BUF_INCREMENT);

		सचे (newbuf == nullptr) {
			CODER_KILL(encoder, "could not grow buffer");
		}

		encoder->buf = newbuf;
		encoder->bufsize += BSON_BUF_INCREMENT;
		debug("allocated %d bytes", BSON_BUF_INCREMENT);
	}

	memcpy(encoder->buf + encoder->bufpos, p, s);
	encoder->bufpos += s;
	debug("appended %zu bytes", s);

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क
write_int8(bson_encoder_t encoder, int8_t b)
{
	पटिनिवत्त write_x(encoder, &b, परिमाण(b));
}

स्थिर पूर्णाङ्क
write_int32(bson_encoder_t encoder, int32_t i)
{
	पटिनिवत्त write_x(encoder, &i, परिमाण(i));
}

स्थिर पूर्णाङ्क
write_int64(bson_encoder_t encoder, int64_t i)
{
	पटिनिवत्त write_x(encoder, &i, परिमाण(i));
}

स्थिर पूर्णाङ्क
write_double(bson_encoder_t encoder, द्विगुण d)
{
	पटिनिवत्त write_x(encoder, &d, परिमाण(d));
}

स्थिर पूर्णाङ्क
write_name(bson_encoder_t encoder, ध्रुव वर्ण *name)
{
	size_t len = strlen(name);

	सचे (len > BSON_MAXNAME) {
		CODER_KILL(encoder, "node name too long");
	}

	पटिनिवत्त write_x(encoder, name, len + 1);
}

पूर्णाङ्क
bson_encoder_init_file(bson_encoder_t encoder, पूर्णाङ्क fd)
{
	encoder->fd = fd;
	encoder->buf = nullptr;
	encoder->dead = मिच्छा;
	encoder->total_document_size = 0;

	सचे (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_init_buf_file(bson_encoder_t encoder, पूर्णाङ्क fd, शून्य *buf, unsigned bufsize)
{
	encoder->fd = fd;
	encoder->buf = (uint8_t *)buf;
	encoder->bufpos = 0;
	encoder->bufsize = bufsize;
	encoder->dead = मिच्छा;
	encoder->realloc_ok = मिच्छा;
	encoder->total_document_size = 0;

	सचे (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_init_buf(bson_encoder_t encoder, शून्य *buf, unsigned bufsize)
{
	encoder->buf = (uint8_t *)buf;

	सचे (encoder->buf == nullptr) {
		encoder->realloc_ok = सच्च;

	} अञ्ञथा {
		encoder->bufsize = bufsize;
	}

	सचे (write_int32(encoder, 0)) {
		CODER_KILL(encoder, "write error on document length");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_fini(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	सचे (write_int8(encoder, BSON_EOO)) {
		CODER_KILL(encoder, "write error on document terminator");
	}

	सचे (encoder->fd > -1 && encoder->buf != nullptr && encoder->bufpos > 0) {
		/* write final buffer to disk */
		पूर्णाङ्क ret = ::लिख(encoder->fd, encoder->buf, encoder->bufpos);

		सचे (ret == (पूर्णाङ्क)encoder->bufpos) {
			encoder->total_document_size += ret;

		} अञ्ञथा {
			CODER_KILL(encoder, "write error");
		}
	}

	// record document size; total_document_size only counts what went to a file
	ध्रुव int32_t bson_doc_bytes = (encoder->fd > -1) ? encoder->total_document_size : (int32_t)encoder->bufpos;
	debug("writing document size %" PRIi32, bson_doc_bytes);

	सचे (encoder->fd > -1) {
		सचे ((lseek(encoder->fd, 0, SEEK_SET) != 0)
		    || (::लिख(encoder->fd, &bson_doc_bytes, परिमाण(bson_doc_bytes)) != परिमाण(bson_doc_bytes))) {

			CODER_KILL(encoder, "write error on document length");
		}

		::fsync(encoder->fd);

	} अञ्ञथा सचे (encoder->buf != nullptr) {
		/* update buffer length */
		memcpy(encoder->buf, &bson_doc_bytes, परिमाण(bson_doc_bytes));
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_buf_size(bson_encoder_t encoder)
{
	CODER_CHECK(encoder);

	सचे (encoder->fd > -1) {
		पटिनिवत्त -1;
	}

	पटिनिवत्त encoder->bufpos;
}

शून्य *
bson_encoder_buf_data(bson_encoder_t encoder)
{
	/* note, no CODER_CHECK here as the caller has to clean up dead buffers */

	सचे (encoder->fd > -1) {
		पटिनिवत्त nullptr;
	}

	पटिनिवत्त encoder->buf;
}

पूर्णाङ्क bson_encoder_append_bool(bson_encoder_t encoder, ध्रुव वर्ण *name, तार्किक value)
{
	CODER_CHECK(encoder);

	सचे (write_int8(encoder, BSON_BOOL) ||
	    write_name(encoder, name) ||
	    write_int8(encoder, value ? 1 : 0)) {
		CODER_KILL(encoder, "write error on BSON_BOOL");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_append_int32(bson_encoder_t encoder, ध्रुव वर्ण *name, int32_t value)
{
	CODER_CHECK(encoder);

	सचे (write_int8(encoder, BSON_INT32) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT32");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_append_int64(bson_encoder_t encoder, ध्रुव वर्ण *name, int64_t value)
{
	CODER_CHECK(encoder);

	सचे (write_int8(encoder, BSON_INT64) ||
	    write_name(encoder, name) ||
	    write_int64(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_INT64");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_append_double(bson_encoder_t encoder, ध्रुव वर्ण *name, द्विगुण value)
{
	CODER_CHECK(encoder);

	सचे (write_int8(encoder, BSON_DOUBLE) ||
	    write_name(encoder, name) ||
	    write_double(encoder, value)) {
		CODER_KILL(encoder, "write error on BSON_DOUBLE");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_append_string(bson_encoder_t encoder, ध्रुव वर्ण *name, ध्रुव वर्ण *string)
{
	size_t len;

	CODER_CHECK(encoder);

	len = strlen(string) + 1;	/* include trailing nul */

	सचे (write_int8(encoder, BSON_STRING) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, len) ||
	    write_x(encoder, string, len)) {
		CODER_KILL(encoder, "write error on BSON_STRING");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_append_द्विमान(bson_encoder_t encoder, ध्रुव वर्ण *name, bson_द्विमान_subtype_t subtype, size_t size,
			   ध्रुव शून्य *data)
{
	CODER_CHECK(encoder);

	सचे (write_int8(encoder, BSON_BINDATA) ||
	    write_name(encoder, name) ||
	    write_int32(encoder, size) ||
	    write_int8(encoder, subtype) ||
	    write_x(encoder, data, size)) {
		CODER_KILL(encoder, "write error on BSON_BINDATA");
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क
bson_encoder_append_null(bson_encoder_t encoder, ध्रुव वर्ण *name)
{
	CODER_CHECK(encoder);

	सचे (write_int8(encoder, BSON_nullptr) ||
	    write_name(encoder, name)) {
		CODER_KILL(encoder, "write error on BSON_nullptr");
	}

	पटिनिवत्त 0;
}
